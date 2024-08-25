#include "cpedestrian.h"
#include "Logic/MapModels/creadonlymap.h"
#include "MapElements/RoadUsers/ccar.h"
#include "MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h"
#include "MapElements/StationaryMapElements/croadelement.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"
#include <random>

CPedestrian::CPedestrian(QVector<QPixmap> pedestrian_states_pixmaps, QString description, EMovementPlane movement_plane,
                         EHorizontalMoveDirection horizontal_move_direction, EVerticalMoveDirection vertical_move_direction) :
    CRoadUser(ERoadUsers::pedestrian, description, movement_plane, horizontal_move_direction, vertical_move_direction),
    m_pedestrian_states_pixmaps(pedestrian_states_pixmaps)
{
    setPixmap(pedestrian_states_pixmaps[0]);
    m_default_cell_size = CReadOnlyMap::get_default_cell_size();

    m_movement_states_mapped_to_func[EPedestrianMovementStates::walking] = &CPedestrian::take_next_step;
    m_movement_states_mapped_to_func[EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_with_lights] =
        &CPedestrian::wait_to_cross_pedestrian_crossing_with_lights;
    m_movement_states_mapped_to_func[EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_without_lights] =
        &CPedestrian::wait_to_cross_pedestrian_crossing_without_lights;
    m_movement_states_mapped_to_func[EPedestrianMovementStates::crossing_pedestrian_crossing] = &CPedestrian::cross_pedestrian_crossing;
    m_movement_states_mapped_to_func[EPedestrianMovementStates::moving_to_destination] = &CPedestrian::move_to_destination;

    m_movement_states_to_destination_mapped_to_func[EPedestrianMovementStatesToDestination::walking_to_destination] =
        &CPedestrian::take_next_step_to_destination;
}

void CPedestrian::set_destination(QPointF destination, std::vector<QPoint> path)
{
    m_has_designated_destination = true;
    m_destination = destination;
    m_path_to_destination = path;

    m_current_movement_state = EPedestrianMovementStates::moving_to_destination;
}

CRoadUser *CPedestrian::create_pedestrian()
{
    return new CPedestrian(QVector<QPixmap>{QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right_still.png"),
                                            QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right.png"),
                                            QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right_2.png")},
                           "Pedestrian", EMovementPlane::horizontal, EHorizontalMoveDirection::right);
}

double CPedestrian::get_possible_acceleration()
{
    double speed_reserve = m_road_users_parameters_adjusted_to_step.m_max_speed - m_current_speed;
    double acceleration_possible = 0;

    if(speed_reserve == 0){
        acceleration_possible = 0;
    }
    else if(speed_reserve > 0.5*m_road_users_parameters_adjusted_to_step.m_max_speed){
        acceleration_possible = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value;
    }
    else{
        acceleration_possible = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value *
                                (speed_reserve/m_road_users_parameters_adjusted_to_step.m_max_speed);
    }

    return acceleration_possible;
}

void CPedestrian::take_next_step()
{
    double acceleration_possible = get_possible_acceleration();

    m_current_speed = m_current_speed + acceleration_possible;
    update_speed_vector();

    QPointF new_possible_pos(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);

    int rotation_increments = 12;
    bool direction_found = false;
    bool p_crossing_collision = false;
    CRoadElement *road_element;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, rotation_increments-2);

    QVector<int> rotations_possible{30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330};

    for(int i = 1; i < rotation_increments; i++){
        auto items_at_pos = m_map->items(new_possible_pos);

        for(auto item : items_at_pos){
            road_element = dynamic_cast<CRoadElement*>(item);

            if(road_element){
                if(road_element->get_permitted_road_users() == EPermittedRoadUsers::all ||
                    road_element->get_permitted_road_users() == EPermittedRoadUsers::pedestrians){

                    if(road_element->get_permitted_road_users() == EPermittedRoadUsers::all && is_omitting_p_crossing){
                        QPointF center = this->boundingRect().center();
                        this->setTransformOriginPoint(center);
                        this->setRotation(rotations_possible[dist(gen)]);
                        update_speed_vector();
                        new_possible_pos = QPoint(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);

                        p_crossing_omission_steps--;
                        if(p_crossing_omission_steps == 0){
                            is_omitting_p_crossing = false;
                        }
                        continue;
                    }

                    QPointF current_pos = this->pos();
                    this->setPos(new_possible_pos);
                    auto colliding_items = m_map->collidingItems(this);
                    this->setPos(current_pos);
                    bool collision_detected = false;

                    for(auto item : colliding_items){
                        auto pedestrian_crossing = dynamic_cast<CPedestrianCrossing*>(item);
                        if(pedestrian_crossing){
                            p_crossing_collision = true;
                            m_p_crossing = pedestrian_crossing;

                            if(is_omitting_p_crossing){
                                p_crossing_omission_steps--;
                                if(p_crossing_omission_steps == 0){
                                    is_omitting_p_crossing = false;
                                }
                            }
                            break;
                        }

                        auto roadway_element = dynamic_cast<CRoadwayElement*>(item);
                        if(roadway_element){
                            collision_detected = true;
                            break;
                        }
                    }

                    if(!collision_detected || (p_crossing_collision && !is_omitting_p_crossing)){
                        direction_found = true;
                        break;
                    }
                }
            }
        }

        if(direction_found){
            break;
        }
        else {
            QPointF center = this->boundingRect().center();
            this->setTransformOriginPoint(center);
            this->setRotation(rotations_possible[dist(gen)]);
            update_speed_vector();
            if(!p_crossing_collision){
                new_possible_pos = QPoint(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
            }
        }
    }

    if(!direction_found){
        //suicide, something went wrong
        return;
    }

    //Pavement
    if(road_element->get_permitted_road_users() == EPermittedRoadUsers::pedestrians && !p_crossing_collision){
        this->setPos(new_possible_pos);
        m_current_movement_state = EPedestrianMovementStates::walking;

        return;
    }

    // pedestrian crossing
    if(p_crossing_collision){
        road_element = m_p_crossing;
    }

    bool is_crossing_with_lights = false;

    int x_offset = 5;
    int y_offset = 5;

    EMovementPlane pc_movement_plane = road_element->get_movement_plane();
    CPavement *pavement = nullptr;

    //Looking for the lights on the RIGHT SIDE OF THE ROAD PEDESTRIAN IS GOING TO CROSS
    if(pc_movement_plane == EMovementPlane::horizontal){
        bool is_pavement = false;
        QPoint pavement_center_pos = QPoint(road_element->pos().x() - m_default_cell_size.width()/2,
                                            road_element->pos().y() + m_default_cell_size.height()/2);

        while(!is_pavement){
            auto items_at_pavement_pos = m_map->items(pavement_center_pos);

            for(auto item : items_at_pavement_pos){
                auto pavement_item = dynamic_cast<CPavement*>(item);
                if(pavement_item){
                    pavement = pavement_item;
                    is_pavement = true;
                    break;
                }
            }

            if(!is_pavement){
                pavement_center_pos = QPoint(pavement_center_pos.x() - m_default_cell_size.width(),
                                             pavement_center_pos.y());
            }
        }
    }
    else{
        bool is_pavement = false;
        QPoint pavement_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                            road_element->pos().y() + m_default_cell_size.height()*1.5);

        while(!is_pavement){
            auto items_at_pavement_pos = m_map->items(pavement_center_pos);

            for(auto item : items_at_pavement_pos){
                auto pavement_item = dynamic_cast<CPavement*>(item);
                if(pavement_item){
                    pavement = pavement_item;
                    is_pavement = true;
                    break;
                }
            }

            if(!is_pavement){
                pavement_center_pos = QPoint(pavement_center_pos.x(),
                                             pavement_center_pos.y() + m_default_cell_size.height());
            }
        }
    }

    QPoint possible_lights_pos = QPoint(pavement->pos().x() + x_offset, pavement->pos().y() + y_offset);

    auto items_at_lights_pos = m_map->items(possible_lights_pos);
    CTrafficLight *traffic_light = nullptr;

    for(auto item : items_at_lights_pos){
        auto tl = dynamic_cast<CTrafficLight*>(item);
        if(tl){
            traffic_light = tl;
            is_crossing_with_lights = true;
            break;
        }
    }

    bool crossing_possible = false;

    if(is_crossing_with_lights){
        crossing_possible = check_if_crossing_pedestrian_crossing_with_lights_is_possible(road_element, traffic_light);

        if(crossing_possible){
            m_current_movement_state = EPedestrianMovementStates::crossing_pedestrian_crossing;
        }
        else{
            m_current_movement_state = EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_with_lights;
            m_current_speed = 0;
        }

        m_p_crossing = road_element;
        m_crossing_light = traffic_light;
    }
    else{
        crossing_possible = check_if_crossing_pedestrian_crossing_without_lights_is_possible(road_element);
        if(crossing_possible){
            m_current_movement_state = EPedestrianMovementStates::crossing_pedestrian_crossing;
        }
        else{
            m_current_movement_state = EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_without_lights;
            m_current_speed = 0;
        }

        m_p_crossing = road_element;
    }
}

void CPedestrian::move_to_destination()
{

}

void CPedestrian::take_next_step_to_destination()
{

}

void CPedestrian::cross_pedestrian_crossing()
{
    if(m_p_crossing->get_movement_plane() == EMovementPlane::horizontal){
        double p_crossing_higher_y_bound = m_p_crossing->pos().y();
        double p_crossing_lower_y_bound = m_p_crossing->pos().y() + m_default_cell_size.height() - this->boundingRect().height();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(p_crossing_lower_y_bound, p_crossing_higher_y_bound);
        double p_crossing_random_y_bound = dist(gen);

        double positive_rotation = this->rotation();
        if(positive_rotation < 0){
            positive_rotation = positive_rotation + 360;
        }

        if(this->pos().y() < p_crossing_higher_y_bound){
            if(positive_rotation < 90 || positive_rotation > 270){ //Moving right
                this->set_rotation_by_center(45);
            }
            else{ //Moving left
                this->set_rotation_by_center(135);
            }
        }
        else if(this->pos().y() > p_crossing_lower_y_bound){
            if(positive_rotation < 90 || positive_rotation > 270){ //Moving right
                this->set_rotation_by_center(-45);
            }
            else{ //Moving left
                this->set_rotation_by_center(-135);
            }
        }
        else if(this->pos().y() < p_crossing_random_y_bound){
            if(positive_rotation < 90 || positive_rotation > 270){ //Moving right
                this->set_rotation_by_center(15);
            }
            else{ //Moving left
                this->set_rotation_by_center(165);
            }
        }
        else if(this->pos().y() > p_crossing_random_y_bound){
            if(positive_rotation < 90 || positive_rotation > 270){ //Moving right
                this->set_rotation_by_center(-15);
            }
            else{ //Moving left
                this->set_rotation_by_center(-165);
            }
        }
    }
    else{
        double p_crossing_left_x_bound = m_p_crossing->pos().x();
        double p_crossing_right_x_bound = m_p_crossing->pos().x() + m_default_cell_size.width() - this->boundingRect().height();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(p_crossing_left_x_bound, p_crossing_right_x_bound);

        double p_crossing_random_x_bound = dist(gen);
        double positive_rotation = this->rotation();
        if(positive_rotation < 0){
            positive_rotation = positive_rotation + 360;
        }

        if(this->pos().x() < p_crossing_left_x_bound){
            if(positive_rotation > 180){ //Moving up
                this->set_rotation_by_center(-45);
            }
            else{ //Moving down
                this->set_rotation_by_center(45);
            }
        }
        else if(this->pos().x() > p_crossing_right_x_bound){
            if(positive_rotation > 180){ //Moving up
                this->set_rotation_by_center(-135);
            }
            else{ //Moving down
                this->set_rotation_by_center(135);
            }
        }
        else if(this->pos().x() < p_crossing_random_x_bound){
            if(positive_rotation > 180){ //Moving up
                this->set_rotation_by_center(-75);
            }
            else{ //Moving down
                this->set_rotation_by_center(75);
            }
        }
        else if(this->pos().x() > p_crossing_random_x_bound){
            if(positive_rotation > 180){ //Moving up
                this->set_rotation_by_center(-105);
            }
            else{ //Moving down
                this->set_rotation_by_center(105);
            }
        }
    }

    double acceleration_possible = get_possible_acceleration();
    m_current_speed = m_current_speed + acceleration_possible;
    update_speed_vector();

    QPointF new_possible_pos(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);

    this->setPos(new_possible_pos);
    bool got_out_of_p_crossing = true;

    auto items_at_pos = m_map->items(new_possible_pos);
    for(auto item : items_at_pos){ //Get out of pedestrian crossing, pavement is assured by validation
        auto p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

        if(p_crossing){
            got_out_of_p_crossing = false;
            break;
        }
    }

    if(got_out_of_p_crossing){
        is_omitting_p_crossing = true;
        p_crossing_omission_steps = 10;
        m_current_movement_state = EPedestrianMovementStates::walking;
    }
}

void CPedestrian::wait_to_cross_pedestrian_crossing_with_lights()
{
    bool is_possible = check_if_crossing_pedestrian_crossing_with_lights_is_possible(m_p_crossing, m_crossing_light);
    if(is_possible){
        m_current_movement_state = EPedestrianMovementStates::crossing_pedestrian_crossing;
    }
}

void CPedestrian::wait_to_cross_pedestrian_crossing_without_lights()
{
    bool is_possible = check_if_crossing_pedestrian_crossing_without_lights_is_possible(m_p_crossing);
    if(is_possible){
        m_current_movement_state = EPedestrianMovementStates::crossing_pedestrian_crossing;
    }
}

bool CPedestrian::check_if_crossing_pedestrian_crossing_with_lights_is_possible(CRoadElement *p_crossing, CTrafficLight *traffic_light)
{
    if(traffic_light->get_pedestrians_lights_phase() == ETrafficLightsPhase::green){
        return true;
    }
    else{
        return false;
    }
}

bool CPedestrian::check_if_crossing_pedestrian_crossing_without_lights_is_possible(CRoadElement *p_crossing)
{
    if(p_crossing->get_movement_plane() == EMovementPlane::horizontal){
        QPoint crossing_center_pos = QPoint(p_crossing->pos().x() + m_default_cell_size.width()/2,
                                            p_crossing->pos().y() + m_default_cell_size.height()/2);
        QGraphicsLineItem *collision_line = new QGraphicsLineItem(crossing_center_pos.x(), crossing_center_pos.y() + 300,
                                        crossing_center_pos.x(), crossing_center_pos.y() - 300);
        m_map->addItem(collision_line);
        auto items_on_collision_line = m_map->collidingItems(collision_line);

        for(auto item : items_on_collision_line){
            auto car = dynamic_cast<CCar*>(item);
            if(car){
                if(car->get_is_waiting()){
                    continue;
                }
                else{
                    m_map->removeItem(collision_line);
                    delete collision_line;
                    return false;
                }
            }
        }

        m_map->removeItem(collision_line);
        delete collision_line;
    }
    else{
        QPoint crossing_center_pos = QPoint(p_crossing->pos().x() + m_default_cell_size.width()/2,
                                            p_crossing->pos().y() + m_default_cell_size.height()/2);
        QGraphicsLineItem *collision_line = new QGraphicsLineItem(crossing_center_pos.x(), crossing_center_pos.y() + 300,
                                                                  crossing_center_pos.x(), crossing_center_pos.y() - 300);
        m_map->addItem(collision_line);
        auto items_on_collision_line = m_map->collidingItems(collision_line);

        for(auto item : items_on_collision_line){
            auto car = dynamic_cast<CCar*>(item);
            if(car){
                if(car->get_is_waiting()){
                    continue;
                }
                else{
                    m_map->removeItem(collision_line);
                    delete collision_line;
                    return false;
                }
            }
        }

        m_map->removeItem(collision_line);
        delete collision_line;
    }

    return true;
}

void CPedestrian::move(CReadOnlyMap *map)
{
    m_map = map;

    auto it = m_movement_states_mapped_to_func.find(m_current_movement_state);
    if (it != m_movement_states_mapped_to_func.end()) {
        (this->*(it.value()))();
    }

    if(m_current_movement_state != EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_with_lights &&
        m_current_movement_state != EPedestrianMovementStates::waiting_to_cross_pedestrian_crossing_without_lights){
        if(m_current_step_state < m_pedestrian_states_pixmaps.size() - 1){
            m_current_step_state++;
        }
        else{
            m_current_step_state = 0;
        }

        setPixmap(m_pedestrian_states_pixmaps[m_current_step_state]);
    }
    else{
        m_current_step_state = 0;
        setPixmap(m_pedestrian_states_pixmaps[m_current_step_state]);
    }
}

