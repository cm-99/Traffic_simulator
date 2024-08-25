#include "csimulationcontroller.h"
#include "MapViews/csimulationview.h"
#include "MapElements/RoadUsers/ccar.h"
#include "MapElements/RoadUsers/cpedestrian.h"
#include <QMouseEvent>
#include <QMessageBox>

CSimulationController::CSimulationController(CApplicationController *application_controller, CEditableMap *map_model, SSimulationConfiguration simulation_configuration) :
    CBaseController(application_controller),
    m_simulation_configuration(simulation_configuration)
{
    m_map_model = map_model;
    m_map_view = new CSimulationView(m_map_model, this);

    prepare_map_mobility_grid();
    auto traffic_lights = m_map_model->get_traffic_lights();

    for(auto traffic_light : *traffic_lights){
        traffic_light->is_automatic_offset_disabled(simulation_configuration.m_lights_random_offset_disabled);
        traffic_light->is_automatic_synchronization_disabled(simulation_configuration.m_lights_automatic_synchronization_disabled);
        traffic_light->set_ligths_durations(simulation_configuration.m_traffic_lights_duration);
    }

    srand(time(NULL));
    std::random_device rd;
    m_generator = new std::mt19937(rd());

    prepare_pedestrians_attributes_distributions();
    prepare_cars_attributes_distributions();

    if(simulation_configuration.m_place_road_users_automatically){
        if(simulation_configuration.m_cars_to_place != 0 || simulation_configuration.m_pedestrians_to_place != 0){
            place_road_users_automatically();
            m_simulation_is_ready = true;
        }
    }

    m_simulation_timer.setInterval(m_simulation_step_interval);
    connect(&m_simulation_timer, &QTimer::timeout, this, &CSimulationController::slot_process_simulation_step);
}

CSimulationController::~CSimulationController()
{
    if(m_map_model != nullptr){
        delete m_map_model;
    }

    if(m_map_view != nullptr){
        delete m_map_view;
    }

    delete m_generator;
    m_pedestrian_attributes_distributions.clear();
    m_cars_attributes_distributions.clear();
}

void CSimulationController::place_road_users_automatically()
{
    if(m_simulation_configuration.m_cars_to_place != 0){
        bool placement_successful = place_cars_automatically();
        if(!placement_successful){
            QMessageBox::warning(nullptr, "", "Failed to automatically place full number of cars");
        }
    }

    if(m_simulation_configuration.m_pedestrians_to_place != 0){
        bool placement_successful = place_pedestrians_automatically();
        if(!placement_successful){
            QMessageBox::warning(nullptr, "", "Failed to automatically place full number of pedestrians");
        }
    }
}

bool CSimulationController::place_pedestrians_automatically()
{
    auto stationary_map_elements = m_map_model->get_stationary_map_elements();
    QVector<CPavement*> pavements;

    for(auto element : *stationary_map_elements){
        auto pavement = dynamic_cast<CPavement*>(element);
        if(pavement){
            pavements.append(pavement);
        }
    }

    int pedestrians_to_place = m_simulation_configuration.m_pedestrians_to_place;

    auto default_size = CReadOnlyMap::get_default_cell_size();
    std::uniform_int_distribution<> dist_rand_position(0, default_size.width());
    std::uniform_int_distribution<> dist(0, pavements.size() - 1);
    std::uniform_int_distribution<> dist_rotations(0, 4);

    for(int i = 0; i < pedestrians_to_place; i++){

        EMapElementPositionValidity pos_validity = EMapElementPositionValidity::invalid;
        auto pedestrian = CPedestrian::create_pedestrian();
        pedestrian->set_basic_parameters(prepare_road_users_parameters(m_pedestrian_attributes_distributions));
        m_map_model->add_road_user(pedestrian, QPoint(0, 0));
        int failsafe = pavements.size()*10;

        while(pos_validity == EMapElementPositionValidity::invalid){
            int pavement_to_check = dist(*m_generator);
            auto pavement = pavements[pavement_to_check];

            QPoint position_to_check = QPoint(pavement->pos().x() + dist_rand_position(*m_generator), pavement->pos().y() + dist_rand_position(*m_generator));
            //Rotate pedestrian
            int rn = dist_rotations(*m_generator);

            for(int i = 0; i < rn; i++){
                rotate_road_user(90, pedestrian);
            }

            QPoint legal_position = get_road_user_legal_position(position_to_check, pedestrian, false);
            pos_validity = get_road_user_position_validity(legal_position, pedestrian);
            if(pos_validity == EMapElementPositionValidity::valid){
                pedestrian->setPos(this->m_map_view->mapToScene(legal_position));
            }
            else{
                failsafe--;
                if(failsafe == 0){
                    m_map_model->erase_road_user(pedestrian);
                    return false;
                }
            }
        }
    }

    return true;
}

void CSimulationController::configure_road_user(CRoadUser *road_user)
{
    if(m_road_user_being_configured != nullptr){
        return;
    }
    m_road_user_being_configured = road_user;

    emit signal_road_user_configuration_requested(road_user->get_basic_parameters(), road_user->get_road_user_type());
}

void CSimulationController::prepare_map_mobility_grid()
{
    auto map_size = m_map_model->get_scene_rect().size();
    auto cell_size = m_map_model->get_default_cell_size();

    int map_cells_rows = map_size.height()/cell_size.height();
    int map_cells_columns = map_size.width()/cell_size.width();

    m_map_mobility_representation.resize(map_cells_rows);
    for(int i = 0; i < map_cells_rows; i++){
        m_map_mobility_representation[i].resize(map_cells_columns);
    }

    for(int i = 0; i < map_cells_rows; i++){
        for(int j = 0; j < map_cells_columns; j++){

            auto cell_center_pos = QPointF(j*cell_size.width() + cell_size.width()/2, i*cell_size.height() + cell_size.height()/2);
            m_map_mobility_representation[i][j] = EPermittedRoadUsers::none;
            auto items_at_pos = m_map_model->items(cell_center_pos);

            for(auto item : items_at_pos){
                auto road_element = dynamic_cast<CRoadElement*>(item);

                if(road_element){
                    m_map_mobility_representation[i][j] = road_element->get_permitted_road_users();
                    break;
                }
            }
        }
    }

}

void CSimulationController::add_road_user_with_destination_being_chosen(CRoadUser *road_user)
{
    if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
        if(road_user->get_road_user_type() != m_current_roads_users_being_selected){
            QMessageBox::warning(nullptr, "", "Road users of different types cannot be mixed in group selection.");
            return;
        }
    }
    else{
        m_current_roads_users_being_selected = road_user->get_road_user_type();
        m_destination_validation_item = new QGraphicsEllipseItem(0, 0, 10, 10);
        m_destination_validation_item->setPos(road_user->pos().x(), road_user->pos().y());
        m_destination_validation_item->setBrush(QBrush(Qt::red, Qt::SolidPattern));
        m_destination_validation_item->setZValue(3);
        m_map_model->addItem(m_destination_validation_item);
    }

    auto it = m_road_users_mapped_to_selection_ellipses.find(road_user);

    //Road user not found, add it
    if(m_road_users_mapped_to_selection_ellipses.find(road_user) == m_road_users_mapped_to_selection_ellipses.end()){
        auto user_size = road_user->boundingRect().size();
        auto circle_radius = (user_size.width() > user_size.height()) ? user_size.width() : user_size.height();
        double x_offset = circle_radius/4;

        auto selection_ellipse = new QGraphicsEllipseItem(0, 0, circle_radius, circle_radius);
        selection_ellipse->setZValue(3);
        selection_ellipse->setPos(road_user->pos().x() - x_offset, road_user->pos().y());

        if(road_user->has_designated_destination()){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(2, 18);
            int random_color_value = dist(gen);
            Qt::GlobalColor random_color = static_cast<Qt::GlobalColor>(random_color_value);
            selection_ellipse->setBrush(QBrush(QColor(random_color), Qt::SolidPattern));

            auto destination_ellipse = new QGraphicsEllipseItem(0, 0, circle_radius, circle_radius);
            destination_ellipse->setZValue(3);
            destination_ellipse->setBrush(QBrush(QColor(random_color), Qt::SolidPattern));
            destination_ellipse->setPos(road_user->get_destination());
            m_map_model->addItem(destination_ellipse);

            m_road_users_mapped_to_current_destination_items.insert(road_user, destination_ellipse);
        }
        else{
            selection_ellipse->setBrush(QBrush(Qt::green, Qt::SolidPattern));
        }

        m_map_model->addItem(selection_ellipse);
        m_road_users_mapped_to_selection_ellipses.insert(road_user, selection_ellipse);
    }
    else{
        auto ellipse = it.value();
        m_map_model->removeItem(ellipse);
        m_road_users_mapped_to_selection_ellipses.remove(it.key());
        delete ellipse;

        auto itt = m_road_users_mapped_to_current_destination_items.find(road_user);
        if(m_road_users_mapped_to_current_destination_items.find(road_user) == m_road_users_mapped_to_current_destination_items.end()){
            auto destination_ellipse = itt.value();
            m_map_model->removeItem(destination_ellipse);
            m_road_users_mapped_to_current_destination_items.remove(itt.key());
            delete destination_ellipse;
        }
    }
}

void CSimulationController::process_road_users_destination_selection(QPointF position)
{
    QPoint destination_mapped_to_grid = map_position_to_mobility_grid(QPoint(position.x(), position.y()));
    qDebug()<<"Destination mapped to grid: "<<destination_mapped_to_grid;
    if(m_map_mobility_representation[destination_mapped_to_grid.x()][destination_mapped_to_grid.y()] == EPermittedRoadUsers::pedestrians){
        qDebug()<<"Pedestrians";
    }
    bool all_paths_found = true;
    int paths_not_found = 0;

    if(validate_destination(position) != EMapElementPositionValidity::valid){
        return;
    }
    else{
        for(auto [key, value] : m_road_users_mapped_to_selection_ellipses.asKeyValueRange()){

            EPermittedRoadUsers permitted_road_user;
            if(key->get_road_user_type() == ERoadUsers::car){
                permitted_road_user = EPermittedRoadUsers::cars;
            }
            else{
                permitted_road_user = EPermittedRoadUsers::pedestrians;
            }

            QPoint position_mapped_to_grid = map_position_to_mobility_grid(QPoint(key->pos().x(), key->pos().y()));
            auto path = m_path_finder.find_path(m_map_mobility_representation, position_mapped_to_grid, destination_mapped_to_grid,
                                                permitted_road_user);

            if(path.empty()){
                all_paths_found = false;
                paths_not_found++;
                continue;
            }

            key->set_destination(position, path);
        }
    }

    if(!all_paths_found){
        QMessageBox::warning(nullptr, "", "Paths could not be found for " + QString::number(paths_not_found) + " road users");
    }

    clear_users_destination_selection();
}

EMapElementPositionValidity CSimulationController::validate_destination(QPointF position)
{
    EPermittedRoadUsers main_road_user_type;

    if(m_current_roads_users_being_selected == ERoadUsers::car){
        main_road_user_type = EPermittedRoadUsers::cars;
    }
    else{
        main_road_user_type = EPermittedRoadUsers::pedestrians;
    }

    auto items_at_pos = m_map_model->items(position);
    bool destination_is_legal = false;

    for(auto item : items_at_pos){
        auto road_element = dynamic_cast<CRoadElement*>(item);

        if(road_element){
            if(road_element->get_permitted_road_users() == main_road_user_type ||
                road_element->get_permitted_road_users() == EPermittedRoadUsers::all){
                destination_is_legal = true;
                break;
            }
        }
    }

    if(destination_is_legal){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::invalid;
    }
}

void CSimulationController::clear_users_destination_selection()
{
    for(auto [key, value] : m_road_users_mapped_to_selection_ellipses.asKeyValueRange()){
        m_map_model->removeItem(value);
        delete value;
    }

    for(auto [key, value] : m_road_users_mapped_to_current_destination_items.asKeyValueRange()){
        m_map_model->removeItem(value);
        delete value;
    }

    m_road_users_mapped_to_current_destination_items.clear();
    m_road_users_mapped_to_selection_ellipses.clear();

    m_map_model->removeItem(m_destination_validation_item);
    delete m_destination_validation_item;
    m_destination_validation_item = nullptr;
}

QPoint CSimulationController::map_position_to_mobility_grid(QPoint position)
{
    auto cell_size = m_map_model->get_default_cell_size();
    int grid_y_pos = std::div(position.x(), cell_size.width()).quot;
    int grid_x_pos = std::div(position.y(), cell_size.height()).quot;

    return QPoint(grid_x_pos, grid_y_pos);
}

bool CSimulationController::place_cars_automatically()
{
    auto stationary_map_elements = m_map_model->get_stationary_map_elements();
    QVector<CRoadway*> roadways;

    for(auto element : *stationary_map_elements){
        auto roadway = dynamic_cast<CRoadway*>(element);
        if(roadway){
            roadways.append(roadway);
        }
    }

    int cars_to_place = m_simulation_configuration.m_cars_to_place;

    std::uniform_int_distribution<> dist(0, roadways.size() - 1);
    std::uniform_int_distribution<> dist_movement_planes(0, 1);

    for(int i = 0; i < cars_to_place; i++){

        EMapElementPositionValidity pos_validity = EMapElementPositionValidity::invalid;
        auto car = CCar::create_car();
        car->set_basic_parameters(prepare_road_users_parameters(m_cars_attributes_distributions));
        m_map_model->add_road_user(car, QPoint(0, 0));
        int failsafe = roadways.size()*3;

        while(pos_validity == EMapElementPositionValidity::invalid){
            int roadway_to_check = dist(*m_generator);
            auto roadway = roadways[roadway_to_check];

            QPoint position_to_check;
            if(roadway->get_movement_plane() == EMovementPlane::vertical){
                int rn = dist_movement_planes(*m_generator);
                if(rn == 0){
                    rotate_road_user(-90, car);
                }
                else{
                    rotate_road_user(90, car);
                }

                position_to_check = QPoint(roadway->pos().x() + CReadOnlyMap::get_default_cell_size().width()/2, roadway->pos().y() + 5);
                car->setPos(position_to_check);
            }
            else{
                int rn = dist_movement_planes(*m_generator);
                if(rn == 0){
                    rotate_road_user(-90, car);
                    rotate_road_user(-90, car);
                }

                position_to_check = QPoint(roadway->pos().x() + CReadOnlyMap::get_default_cell_size().width()/2 + 5, roadway->pos().y() + 5);
                car->setPos(position_to_check);
            }

            QPoint legal_position = get_road_user_legal_position(position_to_check, car, false);
            pos_validity = get_road_user_position_validity(legal_position, car);
            if(pos_validity == EMapElementPositionValidity::valid){
                car->setPos(legal_position);
            }
            else{
                failsafe--;
                if(failsafe == 0){
                    m_map_model->erase_road_user(car);
                    return false;
                }
            }
        }
    }

    return true;
}

void CSimulationController::set_user_to_place_creation_func(CRoadUser *(*creation_func)())
{
    if(!m_simulation_is_paused || !m_road_users_mapped_to_selection_ellipses.isEmpty()){
        return;
    }

    if(m_road_user_is_being_placed){
        m_map_model->erase_road_user(m_road_user_being_placed);
    }

    m_current_creation_func = creation_func;
    m_road_user_being_placed = creation_func();
    m_map_model->add_road_user(m_road_user_being_placed, QPointF(0, 0));
    prepare_validation_rect();
    m_road_user_is_being_placed = true;
}

bool CSimulationController::process_wheel_event(QWheelEvent *event)
{
    if(m_road_user_is_being_placed){
        if(m_road_user_being_placed->is_rotable()){
            rotate_road_user_being_placed(event);
            return true;
        }
    }
    return false;
}

void CSimulationController::process_mouse_move_event(QMouseEvent *event)
{
    QPointF mouse_event_mapped_to_scene = this->m_map_view->mapToScene(event->pos());

    if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
        auto destination_validity = validate_destination(mouse_event_mapped_to_scene);
        if(destination_validity == EMapElementPositionValidity::valid){
            m_destination_validation_item->setBrush(Qt::green);
        }
        else{
            m_destination_validation_item->setBrush(Qt::red);
        }

        int x_offset = m_destination_validation_item->boundingRect().width()/2;
        int y_offset = x_offset;

        m_destination_validation_item->setPos(mouse_event_mapped_to_scene.x() - x_offset, mouse_event_mapped_to_scene.y() - y_offset);
    }

    if(!m_road_user_is_being_placed)
        return;

    QRectF element_bounding_rect = m_road_user_being_placed->boundingRect();
    if(m_road_user_is_being_placed)
    {
        m_road_user_being_placed -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2,
                                       mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2);
        m_validation_rect -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2 - m_val_rect_size_offset/2,
                                  mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2 - m_val_rect_size_offset/2);
        update_validation_rect(event);
    }
}

void CSimulationController::process_mouse_press_event(QMouseEvent *event)
{
    if(m_road_user_being_configured != nullptr || m_traffic_light_being_configured != nullptr){
        return;
    }

    if(m_simulation_is_paused){
        process_mouse_press_event_in_paused_simulation(event);
    }
    else{
        process_mouse_press_event_in_running_simulation(event);
    }
}

void CSimulationController::start_simulation()
{
    if(m_road_user_is_being_placed){
        m_map_model->erase_road_user(m_road_user_being_placed);
        m_road_user_is_being_placed = false;
    }

    auto traffic_lights = m_map_model->get_traffic_lights();
    for(auto traffic_light : *traffic_lights){
        traffic_light->start_simulation();
    }

    m_simulation_is_paused = false;
    m_simulation_timer.start();
}

void CSimulationController::pause_simulation()
{
    auto traffic_lights = m_map_model->get_traffic_lights();
    for(auto traffic_light : *traffic_lights){
        traffic_light->pause_simulation();
    }
    m_simulation_is_paused = true;
    m_simulation_timer.stop();
}

void CSimulationController::resume_simulation()
{
    if(m_road_user_is_being_placed){
        m_map_model->erase_road_user(m_road_user_being_placed);
        m_road_user_is_being_placed = false;
    }

    auto traffic_lights = m_map_model->get_traffic_lights();
    for(auto traffic_light : *traffic_lights){
        traffic_light->resume_simulation();
    }
    m_simulation_is_paused = false;
    m_simulation_timer.start();
}

void CSimulationController::restart_simulation()
{
    pause_simulation();
    m_pedestrian_attributes_distributions.clear();
    m_cars_attributes_distributions.clear();

    prepare_pedestrians_attributes_distributions();
    prepare_cars_attributes_distributions();

    auto traffic_lights = m_map_model->get_traffic_lights();

    for(auto traffic_light : *traffic_lights){
        traffic_light->reset_to_default();
        traffic_light->is_automatic_offset_disabled(m_simulation_configuration.m_lights_random_offset_disabled);
        traffic_light->is_automatic_synchronization_disabled(m_simulation_configuration.m_lights_automatic_synchronization_disabled);
        traffic_light->set_ligths_durations(m_simulation_configuration.m_traffic_lights_duration);
    }

    if(m_simulation_configuration.m_place_road_users_automatically){
        m_map_model->erase_road_users();

        if(m_simulation_configuration.m_cars_to_place != 0 || m_simulation_configuration.m_pedestrians_to_place != 0){
            place_road_users_automatically();
            m_simulation_is_ready = true;
        }
    }
    else{
        auto road_users = m_map_model->get_road_users();
        for(auto road_user : *road_users){
            if(road_user->get_road_user_type() == ERoadUsers::pedestrian){
                road_user->set_basic_parameters(prepare_road_users_parameters(m_pedestrian_attributes_distributions));
            }
            else{
                road_user->set_basic_parameters(prepare_road_users_parameters(m_cars_attributes_distributions));
            }
        }
    }
}

void CSimulationController::set_simulation_configuration(SSimulationConfiguration simulation_configuration)
{
    m_simulation_configuration = simulation_configuration;
    restart_simulation();
}

void CSimulationController::set_simulation_speed(uint simulation_speed)
{
    m_simulation_speed = simulation_speed;
    auto traffic_lights = m_map_model->get_traffic_lights();
    for(auto traffic_light : *traffic_lights){
        traffic_light->set_simulation_speed(simulation_speed);
    }

    if(!m_simulation_is_paused && m_simulation_is_ready){
        m_simulation_timer.start(m_simulation_step_interval/simulation_speed);
    }
}

void CSimulationController::configure_traffic_light(STrafficLightsDuration traffic_lights_duration, int offset, bool is_disabled)
{
    if(traffic_lights_duration.is_empty()){
        m_traffic_light_being_configured = nullptr;
    }

    if(m_traffic_light_being_configured == nullptr){
        return;
    }

    if(m_simulation_configuration.m_lights_random_offset_disabled){
        m_traffic_light_being_configured->set_lights_simulation_offset(offset);
    }

    m_traffic_light_being_configured->set_ligths_durations(traffic_lights_duration, true);
    m_traffic_light_being_configured->set_is_active(!is_disabled);

    m_traffic_light_being_configured = nullptr;
}

void CSimulationController::configure_road_user(SRoadUsersBasicParameters parameters)
{
    if(parameters.is_empty()){
        m_road_user_being_configured = nullptr;
    }

    if(m_road_user_being_configured == nullptr){
        return;
    }

    m_road_user_being_configured->set_basic_parameters(parameters);
    m_road_user_being_configured = nullptr;
}

void CSimulationController::place_user_and_prepare_next(QMouseEvent *event)
{
    QPoint placement_position = get_road_user_legal_position(event->pos(), m_road_user_being_placed);

    if(get_road_user_position_validity(event->pos(), m_road_user_being_placed) == EMapElementPositionValidity::invalid){
        QMessageBox msg_box;
        msg_box.setText("Selected road user position is not valid.");
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.setDefaultButton(QMessageBox::Ok);
        msg_box.exec();
        return;
    }
    m_road_user_being_placed->setPos(placement_position);

    if(m_road_user_is_being_edited){
        m_road_user_is_being_edited = false;
        m_road_user_is_being_placed = false;
        m_map_model->erase_item(m_validation_rect);
        event->accept();
        return;
    }
    else{
        if(m_road_user_being_placed->get_road_user_type() == ERoadUsers::pedestrian){
            m_road_user_being_placed->set_basic_parameters(prepare_road_users_parameters(m_pedestrian_attributes_distributions));
        }
        else{
            m_road_user_being_placed->set_basic_parameters(prepare_road_users_parameters(m_cars_attributes_distributions));
        }
    }

    qreal previous_element_rotation = (int)m_road_user_being_placed->rotation();
    auto new_road_user_to_place = m_current_creation_func();

    new_road_user_to_place->set_current_movement_plane(m_road_user_being_placed->get_current_movement_plane());
    new_road_user_to_place->set_horizontal_move_direction(m_road_user_being_placed->get_horizontal_move_direction());
    new_road_user_to_place->set_vertical_move_direction(m_road_user_being_placed->get_vertical_move_direction());

    m_road_user_being_placed = new_road_user_to_place;
    QPointF mouse_event_mapped_to_scene = this->m_map_view->mapToScene(event->pos());
    QPointF new_element_pos = QPointF(mouse_event_mapped_to_scene.x() - m_road_user_being_placed->boundingRect().toRect().width()/2,
                                      mouse_event_mapped_to_scene.y() - m_road_user_being_placed->boundingRect().toRect().height()/2);
    m_map_model->add_road_user(m_road_user_being_placed, new_element_pos);
    QPointF center = m_road_user_being_placed->boundingRect().center();
    m_road_user_being_placed->setTransformOriginPoint(center);
    m_road_user_being_placed->setRotation(previous_element_rotation);
    update_validation_rect(event);
    m_simulation_is_ready = true;
}

QPoint CSimulationController::get_road_user_legal_position(QPoint position, CRoadUser *road_user, bool map_to_scene)
{
    QPointF mouse_event_mapped_to_scene;
    if(map_to_scene){
        mouse_event_mapped_to_scene = this->m_map_view->mapToScene(position);
    }
    else{
        mouse_event_mapped_to_scene = QPointF(position.x(), position.y());
    }

    if(road_user->get_road_user_type() == ERoadUsers::pedestrian){
        return QPoint(mouse_event_mapped_to_scene.x(), mouse_event_mapped_to_scene.y());
    }
    else{
        auto colliding_items = m_map_model->collidingItems(road_user);
        CRoadway *roadway = nullptr;
        for(auto colliding_item : colliding_items){
            auto maybe_roadway = dynamic_cast<CRoadway*>(colliding_item);
            if(maybe_roadway){
                roadway = maybe_roadway;
                break;
            }
        }

        if(roadway == nullptr){
            return QPoint(-1, -1);
        }

        auto rodaway_test = roadway->boundingRect();
        auto roadway_rect = rodaway_test.toRect();
        auto roadway_width = roadway_rect.width();
        auto roadway_height = roadway_rect.height();

        int x_roadway_offset = 0;
        int y_roadway_offset = 0;

        if(abs((int)roadway->rotation()) % 180 == 90 && roadway_height != roadway_width){
            x_roadway_offset = abs(roadway_width - roadway_height)/2;
            y_roadway_offset = abs(roadway_width - roadway_height)/2;
            roadway_width = roadway_height;
            roadway_height = roadway_rect.width();
        }

        QPoint real_roadway_pos = QPoint(roadway->pos().x() + x_roadway_offset, roadway->pos().y() - y_roadway_offset);

        auto car_bounding_rect = road_user->boundingRect();
        auto car_rect = car_bounding_rect.toRect();
        auto car_width = car_rect.width();
        auto car_height = car_rect.height();

        int x_car_offset = 0;
        int y_car_offset = 0;

        if(abs((int)road_user->rotation()) % 180 == 90 && car_width != car_height){
            x_car_offset = abs(car_width - car_height)/2;
            y_car_offset = abs(car_width - car_height)/2;
        }

        if((int)(road_user->rotation()) % 180 != 0){
            car_width = road_user->boundingRect().height();
            car_height = road_user->boundingRect().width();
        }

        //Place car on the proper lane
        if(road_user->get_current_movement_plane() == EMovementPlane::vertical){
            int y_car_pos = mouse_event_mapped_to_scene.y();
            if(road_user->get_vertical_move_direction() == EVerticalMoveDirection::up){
                int x_car_pos = real_roadway_pos.x() - x_car_offset + roadway_width - car_width - cars_roadside_offset;
                return QPoint(x_car_pos, y_car_pos);
            }
            else{
                int x_car_pos = real_roadway_pos.x() - x_car_offset + cars_roadside_offset;
                return QPoint(x_car_pos, y_car_pos);
            }
        }
        else{
            int x_car_pos = mouse_event_mapped_to_scene.x();
            if(road_user->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                int y_car_pos = real_roadway_pos.y() + roadway_height - car_height - cars_roadside_offset - y_car_offset;
                return QPoint(x_car_pos, y_car_pos);
            }
            else{
                int y_car_pos = real_roadway_pos.y() + cars_roadside_offset - y_car_offset;
                return QPoint(x_car_pos, y_car_pos);
            }
        }
    }
}

void CSimulationController::erase_selected_user(QMouseEvent *event)
{
    if(m_road_user_is_being_placed){
        m_road_user_is_being_placed = false;
        m_map_model->erase_road_user(m_road_user_being_placed);
        m_map_model->erase_item(m_validation_rect);
    }
    else{
        auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
        auto *road_user = dynamic_cast<CRoadUser*>(item_at_mouse_pos);

        if(road_user != nullptr){
            m_map_model->erase_road_user(road_user);
        }
    }
}

std::piecewise_linear_distribution<> CSimulationController::create_road_users_attribute_distribution(double min, double mean, double max)
{
    std::array<double, 3> x = {{min, mean, max}};
    std::array<double, 3> p = {{0, 1, 0}};

    return std::piecewise_linear_distribution<>(x.begin(), x.end(), p.begin());;
}

void CSimulationController::prepare_pedestrians_attributes_distributions()
{
    for(int i = 0; i < SRoadUsersBasicParameters::get_attributes_number(); i++){
        double min = m_simulation_configuration.m_pedestrians_min_basic_parameters.get_attribute_by_index(i);
        double mean = m_simulation_configuration.m_pedestrians_mean_basic_parameters.get_attribute_by_index(i);
        double max = m_simulation_configuration.m_pedestrians_max_basic_parameters.get_attribute_by_index(i);

        m_pedestrian_attributes_distributions.append(create_road_users_attribute_distribution(min, mean, max));
    }
}

void CSimulationController::prepare_cars_attributes_distributions()
{
    for(int i = 0; i < SRoadUsersBasicParameters::get_attributes_number(); i++){
        double min = m_simulation_configuration.m_cars_min_basic_parameters.get_attribute_by_index(i);
        double mean = m_simulation_configuration.m_cars_mean_basic_parameters.get_attribute_by_index(i);
        double max = m_simulation_configuration.m_cars_max_basic_parameters.get_attribute_by_index(i);

        m_cars_attributes_distributions.append(create_road_users_attribute_distribution(min, mean, max));
    }
}

SRoadUsersBasicParameters CSimulationController::prepare_road_users_parameters(QVector<std::piecewise_linear_distribution<>> &attributes_distributions)
{
    QVector<double> attributes_list;
    for(auto dist : attributes_distributions){
        attributes_list.append(dist(*m_generator));
    }

    return SRoadUsersBasicParameters(attributes_list);
}

void CSimulationController::prepare_validation_rect()
{
    m_validation_rect = new QGraphicsRectItem(0, 0, m_road_user_being_placed->boundingRect().width() + m_val_rect_size_offset,
                                              m_road_user_being_placed->boundingRect().height() + m_val_rect_size_offset);
    m_validation_rect->setBrush(Qt::red);
    m_validation_rect->setZValue(4);
    m_validation_rect->setOpacity(0.4);
    m_map_model->addItem(m_validation_rect);
}

EMapElementPositionValidity CSimulationController::get_road_user_position_validity(QPoint position, CRoadUser *road_user, bool map_to_scene)
{
    auto *dummy_road_user = new QGraphicsPixmapItem();
    dummy_road_user->setPixmap(road_user->pixmap());
    dummy_road_user->setVisible(true);
    QPoint placement_pos = get_road_user_legal_position(position, road_user, map_to_scene);

    if(placement_pos.x() == -1){
        return EMapElementPositionValidity::invalid;
    }

    dummy_road_user->setPos(placement_pos);
    if(road_user->rotation() != 0){
        dummy_road_user->setTransformOriginPoint(dummy_road_user->boundingRect().center());
        dummy_road_user->setRotation(road_user->rotation());
    }

    m_map_model->addItem(dummy_road_user);
    auto colliding_items = m_map_model->collidingItems(dummy_road_user);
    m_map_model->removeItem(dummy_road_user);
    delete dummy_road_user;

    bool at_least_one_proper_foundation = false;

    if(road_user->get_road_user_type() == ERoadUsers::car){
        for(auto colliding_item : colliding_items){

            if(colliding_item == m_validation_rect){
                continue;
            }

            auto colliding_road_user = dynamic_cast<CRoadUser*>(colliding_item);
            if(colliding_road_user && colliding_road_user != road_user){
                return EMapElementPositionValidity::invalid;
            }

            auto map_element = dynamic_cast<CStationaryMapElement*>(colliding_item);
            if(!map_element){
                continue;
            }

            EStationaryMapElementType map_element_type = map_element->get_map_element_type();

            if(map_element_type == EStationaryMapElementType::traffic_control_element){
                return EMapElementPositionValidity::invalid;
            }
            else if(map_element_type == EStationaryMapElementType::filler){
                continue;
            }

            CRoadElement *road_element = static_cast<CRoadElement*>(map_element);

            if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
                if(road_element->get_movement_plane() == road_user->get_current_movement_plane()){
                    return EMapElementPositionValidity::invalid;
                }
                else{
                    at_least_one_proper_foundation = true;
                    continue;
                }
            }

            if(road_element->get_permitted_road_users() == EPermittedRoadUsers::pedestrians){
                return EMapElementPositionValidity::invalid;
            }
            else{
                auto roadway_element = static_cast<CRoadwayElement*>(road_element);
                if(!roadway_element){
                    continue;
                }

                if(roadway_element->get_roadway_element_type() != ERoadwayElementType::roadway
                    || roadway_element->get_movement_plane() != road_user->get_current_movement_plane()){
                    return EMapElementPositionValidity::invalid;
                }
                else{
                    at_least_one_proper_foundation = true;
                }
            }
        }
    }
    else{
        for(auto colliding_item : colliding_items){

            if(colliding_item == m_validation_rect){
                continue;
            }

            auto colliding_road_user = dynamic_cast<CRoadUser*>(colliding_item);
            if(colliding_road_user && colliding_road_user != road_user){
                return EMapElementPositionValidity::invalid;
            }

            auto map_element = dynamic_cast<CStationaryMapElement*>(colliding_item);
            if(!map_element){
                continue;
            }

            EStationaryMapElementType map_element_type = map_element->get_map_element_type();

            if(map_element_type == EStationaryMapElementType::traffic_control_element){
                return EMapElementPositionValidity::invalid;
            }
            else if(map_element_type == EStationaryMapElementType::filler){
                continue;
            }

            CRoadElement *road_element = static_cast<CRoadElement*>(map_element);
            if(!road_element){
                continue;
            }

            if(road_element->get_road_element_type() != ERoadElementType::pavement){
                return EMapElementPositionValidity::invalid;
            }
            else{
                at_least_one_proper_foundation = true;
            }
        }
    }

    if(at_least_one_proper_foundation){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::invalid;
    }
}

void CSimulationController::update_validation_rect(QMouseEvent *event)
{
    auto pos_validity = get_road_user_position_validity(event->pos(), m_road_user_being_placed);

    if(pos_validity == EMapElementPositionValidity::valid){
        m_validation_rect -> setBrush(Qt::green);
    }
    else{
        m_validation_rect -> setBrush(Qt::red);
    }
}

void CSimulationController::process_mouse_press_event_in_paused_simulation(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
            if(event->modifiers() & Qt::ShiftModifier){
                auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
                auto *road_user = dynamic_cast<CRoadUser*>(item_at_mouse_pos);

                if(road_user){
                    add_road_user_with_destination_being_chosen(road_user);
                }

                return;
            }
            else{
                process_road_users_destination_selection(this->m_map_view->mapToScene(event->pos()));
                return;
            }
        }

        if(m_road_user_is_being_placed){
            place_user_and_prepare_next(event);
        }
        else{
            auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
            auto *road_user = dynamic_cast<CRoadUser*>(item_at_mouse_pos);

            if(road_user){
                if(event->modifiers() & Qt::ControlModifier){
                    configure_road_user(road_user);
                    return;
                }
                else if(event->modifiers() & Qt::ShiftModifier){
                    add_road_user_with_destination_being_chosen(road_user);
                    return;
                }

                m_road_user_being_placed = road_user;
                m_road_user_is_being_placed = true;
                m_road_user_is_being_edited = true;
                prepare_validation_rect();
                update_validation_rect(event);

                m_validation_rect->setTransformOriginPoint(m_validation_rect->boundingRect().center());
                m_validation_rect->setRotation(m_road_user_being_placed->rotation());
                return;
            }

            auto traffic_light = dynamic_cast<CTrafficLight*>(item_at_mouse_pos);

            if(traffic_light){
                if(m_traffic_light_being_configured != nullptr){
                    return;
                }
                m_traffic_light_being_configured = traffic_light;
                STrafficLightsDuration tld = traffic_light->get_traffic_lights_durations();
                if(tld.full_lights_cycle_time() > 1000){
                    tld.m_green_ligth_duration = tld.m_green_ligth_duration/1000;
                    tld.m_red_ligth_duration = tld.m_red_ligth_duration/1000;
                    tld.m_yellow_ligth_duration = tld.m_yellow_ligth_duration/1000;
                    tld.m_red_and_yellow_ligths_duration = tld.m_red_and_yellow_ligths_duration/1000;
                }
                emit signal_traffic_light_configuration_requested(tld, m_simulation_configuration.m_lights_random_offset_disabled,
                                                                  !(traffic_light->is_active()));
            }
        }
    }
    else if(event->button() == Qt::RightButton){
        if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
            clear_users_destination_selection();
        }
        erase_selected_user(event);
    }
}

void CSimulationController::process_mouse_press_event_in_running_simulation(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
            if(event->modifiers() & Qt::ShiftModifier){
                auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
                auto *road_user = dynamic_cast<CRoadUser*>(item_at_mouse_pos);

                if(road_user){
                    add_road_user_with_destination_being_chosen(road_user);
                }

                return;
            }
            else{
                process_road_users_destination_selection(this->m_map_view->mapToScene(event->pos()));
                return;
            }
        }

        auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
        auto *road_user = dynamic_cast<CRoadUser*>(item_at_mouse_pos);

        if(road_user){
            if(event->modifiers() & Qt::ShiftModifier){
                add_road_user_with_destination_being_chosen(road_user);
                return;
            }

            configure_road_user(road_user);
            return;
        }

        auto traffic_light = dynamic_cast<CTrafficLight*>(item_at_mouse_pos);

        if(traffic_light){
            if(m_traffic_light_being_configured != nullptr){
                return;
            }
            m_traffic_light_being_configured = traffic_light;
            STrafficLightsDuration tld = traffic_light->get_traffic_lights_durations();
            if(tld.full_lights_cycle_time() > 1000){
                tld.m_green_ligth_duration = tld.m_green_ligth_duration/1000;
                tld.m_red_ligth_duration = tld.m_red_ligth_duration/1000;
                tld.m_yellow_ligth_duration = tld.m_yellow_ligth_duration/1000;
                tld.m_red_and_yellow_ligths_duration = tld.m_red_and_yellow_ligths_duration/1000;
            }
            emit signal_traffic_light_configuration_requested(tld, m_simulation_configuration.m_lights_random_offset_disabled,
                                                                !(traffic_light->is_active()));
        }
    }
    else if(event->button() == Qt::RightButton){
        if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){
            clear_users_destination_selection();
        }
    }
}

void CSimulationController::rotate_road_user_being_placed(QWheelEvent *event)
{
    int rotation_delta = event->angleDelta().y() / 90;
    qreal current_rotation = (int)m_road_user_being_placed->rotation() % 360;
    int new_rotation = qRound(current_rotation / 90) * 90 + rotation_delta * 90;
    int rotation_change = new_rotation - current_rotation; //either 90 or -90

    rotate_road_user(rotation_change, m_road_user_being_placed);

    m_validation_rect->setTransformOriginPoint(m_validation_rect->boundingRect().center());
    m_validation_rect->setRotation(new_rotation);
}

void CSimulationController::rotate_road_user(int rotation_change, CRoadUser *road_user)
{
    if(road_user->get_current_movement_plane() == EMovementPlane::horizontal){
        road_user->set_current_movement_plane(EMovementPlane::vertical);
        if(road_user->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            if(rotation_change > 0){
                road_user->set_vertical_move_direction(EVerticalMoveDirection::down);
            }
            else{
                road_user->set_vertical_move_direction(EVerticalMoveDirection::up);
            }
        }
        else{
            if(rotation_change > 0){
                road_user->set_vertical_move_direction(EVerticalMoveDirection::up);
            }
            else{
                road_user->set_vertical_move_direction(EVerticalMoveDirection::down);
            }
        }
    }
    else{
        road_user->set_current_movement_plane(EMovementPlane::horizontal);
        if(road_user->get_vertical_move_direction() == EVerticalMoveDirection::up){
            if(rotation_change > 0){
                road_user->set_horizontal_move_direction(EHorizontalMoveDirection::right);
            }
            else{
                road_user->set_horizontal_move_direction(EHorizontalMoveDirection::left);
            }
        }
        else{
            if(rotation_change > 0){
                road_user->set_horizontal_move_direction(EHorizontalMoveDirection::left);
            }
            else{
                road_user->set_horizontal_move_direction(EHorizontalMoveDirection::right);
            }
        }
    }

    QPointF center = road_user->boundingRect().center();
    road_user->setTransformOriginPoint(center);
    road_user->setRotation((((int)road_user->rotation() % 360) + rotation_change) % 360);
}

void CSimulationController::slot_process_simulation_step()
{
    auto road_users = m_map_model->get_road_users();
    for(auto road_user : *road_users){
        road_user->move(m_map_model);

        if(!m_road_users_mapped_to_selection_ellipses.isEmpty()){

            auto it = m_road_users_mapped_to_selection_ellipses.find(road_user);
            if(it != m_road_users_mapped_to_selection_ellipses.end()){
                it.value()->setPos(road_user->pos());
            }
        }
    }
}
