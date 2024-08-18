#include <QWheelEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QProgressDialog>

#include "cmapcreationcontroller.h"
#include "Logic/MapImportAndExportManagers/cxmlmapimportandexportmanager.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/cturn.h"
#include "MapElements/StationaryMapElements/cfiller.h"
#include "MapViews/cmapcreationview.h"
#include "capplicationcontroller.h"

CMapCreationController::CMapCreationController(CApplicationController *application_controller):
    CBaseController(application_controller)
{
    m_map_model = new CEditableMap(1920, 1080);
    m_map_model->fill_map();
    add_guide_grid();
    m_map_view = new CMapCreationView(m_map_model, this);

    auto *xml_map_manager = new CXMLMapImportAndExportManager();
    m_map_formats_mapped_to_managers.insert(xml_map_manager->get_supported_format(), xml_map_manager);
    m_supported_map_file_formats.append(xml_map_manager->get_supported_format());
}

CMapCreationController::~CMapCreationController()
{
    if(m_map_model != nullptr){
        delete m_map_model;
    }

    if(m_map_view != nullptr){
        delete m_map_view;
    }
}

bool CMapCreationController::load_map_into_creator(QString map_file_path)
{
    auto map_model = load_map_from_file(map_file_path);
    if(!map_model){
        return false;
    }

    set_model(map_model);
    add_guide_grid();

    return true;
}

bool CMapCreationController::delegate_map_saving(QString map_file_path)
{
    if(!perform_final_map_validation()){
        return false;
    }

    QFileInfo map_file_info{map_file_path};
    QString suffix = map_file_info.completeSuffix();
    if(suffix.contains(".")){
        suffix = suffix.split(".").last();
    }
    QString map_file_format = "*." + suffix;
    auto *map_import_manager = m_map_formats_mapped_to_managers.value(map_file_format);

    return map_import_manager -> save_map(m_map_model, map_file_path);
}

void CMapCreationController::clear_map()
{
    for (auto [key, value] : m_elemenets_mapped_to_validation_rects.asKeyValueRange()) {
        m_map_model->erase_item(value);
    }
    m_elemenets_mapped_to_validation_rects.clear();

    m_map_model->clear_stationary_map_elements();
}

bool CMapCreationController::perform_final_map_validation()
{
    auto map_items = *m_map_model->get_stationary_map_elements();
    int items_count = map_items.size();
    QProgressDialog progress_dialog("Validating items...", "Cancel", 0, items_count);
    progress_dialog.setWindowModality(Qt::WindowModal);

    progress_dialog.show();
    for(int i = 0; i < items_count; i++){
        auto item = map_items[i];
        QPoint placement_pos = QPoint(item->pos().x(), item->pos().y());
        auto item_pos_validity = get_element_position_validity_in_relation_to_surroundings_by_serialization(item, placement_pos);
        progress_dialog.setValue(i+1);

        if(item_pos_validity == EMapElementPositionValidity::initially_valid || item_pos_validity == EMapElementPositionValidity::invalid){
            if(!m_elemenets_mapped_to_validation_rects.contains(item)){
                auto validation_rect = prepare_final_validation_rect(item);
                m_elemenets_mapped_to_validation_rects.insert(item, validation_rect);
            }
        }
    }
    progress_dialog.setValue(items_count);

    if(!m_elemenets_mapped_to_validation_rects.isEmpty()){
        QMessageBox msg_box;
        QString text = "Detected " + QString::number(m_elemenets_mapped_to_validation_rects.count()) + " item(s) with incorrect position.\n"
                        "Verify highlighted items positions before proceeding.";
        msg_box.setText(text);
        msg_box.setStandardButtons(QMessageBox::Ok);

        msg_box.exec();
        return false;
    }

    return true;
}


void CMapCreationController::add_guide_grid()
{
    QSize default_element_size = CStationaryMapElement::get_default_map_element_size();
    int default_element_width = default_element_size.width();
    int default_element_height = default_element_size.height();
    int map_model_width = m_map_model->width();
    int map_model_height = m_map_model->height();
    int horizontal_guide_lines_number = map_model_height/default_element_height;
    int vertical_guide_lines_number = map_model_width/default_element_width;

    for(int i = 0; i <= horizontal_guide_lines_number; i++){
        int line_y_pos = i * default_element_height;
        m_map_model->addLine(0, line_y_pos, map_model_width, line_y_pos);
    }


    for(int i = 0; i <= vertical_guide_lines_number; i++){
        int line_x_pos = i * default_element_width;
        m_map_model->addLine(line_x_pos, 0, line_x_pos, map_model_height);
    }
}

void CMapCreationController::place_element_and_prepare_next(QMouseEvent *event)
{
    QPoint placement_position = get_element_placement_position_snapped_to_grid(m_element_being_placed);

    if(get_element_position_validity(m_element_being_placed, placement_position) == EMapElementPositionValidity::invalid){
        QMessageBox msg_box;
        msg_box.setText("Selected element position is not valid.");
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.setDefaultButton(QMessageBox::Ok);
        msg_box.exec();
        return;
    }
    m_element_being_placed->setPos(placement_position);

    if(m_element_is_being_edited){
        m_element_is_being_edited = false;
        m_element_is_being_placed = false;
        m_map_model->erase_item(m_validation_rect);
        event->accept();
        return;
    }

    qreal previous_element_rotation = (int)m_element_being_placed->rotation();
    auto new_element_to_place = m_current_creation_func();

    if(new_element_to_place->get_map_element_type() == EStationaryMapElementType::road_element){
        auto new_road_element = static_cast<CRoadElement*>(new_element_to_place);
        new_road_element->set_movement_plane(static_cast<CRoadElement*>(m_element_being_placed)->get_movement_plane());

        auto new_turn_element = dynamic_cast<CTurn*>(new_road_element);
        if(new_turn_element != nullptr){
            auto old_turn_element = static_cast<CTurn*>(m_element_being_placed);
            new_turn_element->set_horizontal_direction_possible(old_turn_element->get_horizontal_direction_possible());
            new_turn_element->set_vertical_direction_possible(old_turn_element->get_vertical_direction_possible());
        }
    }

    m_element_being_placed = new_element_to_place;
    QPointF mouse_event_mapped_to_scene = this->m_map_view->mapToScene(event->pos());
    QPointF new_element_pos = QPointF(mouse_event_mapped_to_scene.x() - m_element_being_placed->boundingRect().toRect().width()/2,
                                      mouse_event_mapped_to_scene.y() - m_element_being_placed->boundingRect().toRect().height()/2);
    m_map_model->add_stationary_map_element(m_element_being_placed, new_element_pos);
    QPointF center = m_element_being_placed->boundingRect().center();
    m_element_being_placed->setTransformOriginPoint(center);
    m_element_being_placed->setRotation(previous_element_rotation);
    update_validation_rect();
}

void CMapCreationController::prepare_validation_rect()
{
    m_validation_rect = new QGraphicsRectItem(0, 0, m_element_being_placed->boundingRect().width() + m_val_rect_size_offset,
                                              m_element_being_placed->boundingRect().height() + m_val_rect_size_offset);
    m_validation_rect->setBrush(Qt::red);
    m_validation_rect->setZValue(4);
    m_validation_rect->setOpacity(0.4);
    m_map_model->addItem(m_validation_rect);
}

QGraphicsRectItem* CMapCreationController::prepare_final_validation_rect(CStationaryMapElement *item)
{

    auto validation_rect = new QGraphicsRectItem(0, 0, item->boundingRect().width() + m_val_rect_size_offset,
                                              item->boundingRect().height() + m_val_rect_size_offset);
    validation_rect->setBrush(Qt::yellow);
    validation_rect->setZValue(4);
    validation_rect->setOpacity(0.4);
    validation_rect->setPos(item->pos().x() - m_val_rect_size_offset/2, item->pos().y() - m_val_rect_size_offset/2);
    m_map_model->addItem(validation_rect);

    if((int)item->rotation()%180 != 0){
        validation_rect->setTransformOriginPoint(validation_rect->boundingRect().center());
        validation_rect->setRotation((int)item->rotation());
    }

    return validation_rect;
}

void CMapCreationController::set_element_to_place_creation_func(CStationaryMapElement *(*creation_func)())
{
    if(m_element_is_being_placed){
        m_map_model->erase_stationary_map_element(m_element_being_placed);
    }
    else
        m_element_is_being_placed = true;

    m_current_creation_func = creation_func;
    m_element_being_placed = creation_func();
    m_map_model->add_stationary_map_element(m_element_being_placed, QPointF(0, 0));
    prepare_validation_rect();
}

bool CMapCreationController::process_wheel_event(QWheelEvent *event)
{
    if(m_element_is_being_placed){
        if(m_element_being_placed->is_rotable()){
            rotate_element_being_placed(event);
            return true;
        }
    }
    return false;
}

void CMapCreationController::update_validation_rect()
{
    auto pos_validity = get_element_position_validity(m_element_being_placed, get_element_placement_position_snapped_to_grid(m_element_being_placed));

    if(pos_validity == EMapElementPositionValidity::valid){
        m_validation_rect -> setBrush(Qt::green);
    }
    else if(pos_validity == EMapElementPositionValidity::initially_valid){
        m_validation_rect -> setBrush(Qt::yellow);
    }
    else{
        m_validation_rect -> setBrush(Qt::red);
    }
}

void CMapCreationController::process_mouse_move_event(QMouseEvent *event)
{
    if(!m_element_is_being_placed)
        return;

    QPointF mouse_event_mapped_to_scene = this->m_map_view->mapToScene(event->pos());
    QRectF element_bounding_rect = m_element_being_placed->boundingRect();
    if(m_element_is_being_placed)
    {
        m_element_being_placed -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2,
                                       mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2);
        m_validation_rect -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2 - m_val_rect_size_offset/2,
                                  mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2 - m_val_rect_size_offset/2);
        update_validation_rect();
    }
}

void CMapCreationController::process_mouse_press_event(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(m_element_is_being_placed){
            place_element_and_prepare_next(event);
        }
        else{
            auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
            auto *map_element = dynamic_cast<CStationaryMapElement*>(item_at_mouse_pos);

            if(map_element == nullptr){
                auto final_validation_rect = dynamic_cast<QGraphicsRectItem*>(item_at_mouse_pos);

                if(final_validation_rect != nullptr){
                    auto element = m_elemenets_mapped_to_validation_rects.key(final_validation_rect);
                    m_elemenets_mapped_to_validation_rects.remove(element);
                    map_element = element;
                    m_map_model->erase_item(final_validation_rect);
                }
                else{
                    return;
                }
            }
            else if(!is_item_movable(map_element)){
                return;
            }

            m_element_being_placed = map_element;
            m_element_is_being_placed = true;
            m_element_is_being_edited = true;
            prepare_validation_rect();
            update_validation_rect();

            m_validation_rect->setTransformOriginPoint(m_validation_rect->boundingRect().center());
            m_validation_rect->setRotation(m_element_being_placed->rotation());
        }
    }
    else if(event->button() == Qt::RightButton){
        erase_selected_element(event);
    }
}

CEditableMap *CMapCreationController::load_map_from_file(QString map_file_path)
{
    QFileInfo map_file_info{map_file_path};
    QString suffix = map_file_info.completeSuffix();
    if(suffix.contains(".")){
        suffix = suffix.split(".").last();
    }
    QString map_file_format = "*." + suffix;

    auto *map_export_manager = m_map_formats_mapped_to_managers.value(map_file_format);
    auto map_model = map_export_manager ->load_map(map_file_path);
    if(map_model == nullptr){
        QMessageBox::warning(nullptr, "Loading unsuccessful", "Could not open selected file or file format is not supported.");
        return nullptr;
    }

    map_model->fill_map();

    return map_model;
}

void CMapCreationController::process_simulation_start_request()
{
    //TODO: Implement map copy constructor <=> implement copy constructors for all map elements :/
    QString temp_map_file_path = qApp->applicationDirPath() + "/temp.xml";

    if(!delegate_map_saving(temp_map_file_path)){
        return;
    }

    m_application_controller->process_simulation_start_request(temp_map_file_path);

    QFile temp_file(temp_map_file_path);
    temp_file.remove();
}

EMapElementPositionValidity CMapCreationController::get_road_element_position_validity_in_relation_to_foundations(CRoadElement *r_element, const QList<QGraphicsItem *> &colliding_items)
{
    for(auto colliding_item : colliding_items){
        if(colliding_item == r_element)
            continue;

        auto map_element = dynamic_cast<CStationaryMapElement*>(colliding_item);
        if(map_element == nullptr || map_element->get_map_element_type() == EStationaryMapElementType::filler)
            continue;

        //TODO: bandain bugfix - somehow in this specific combination roadway is deemed to be colliding with turn,
        //even though visibly it is not
        auto turn = dynamic_cast<CTurn*>(colliding_item);
        if(turn != nullptr){
            auto roadway = dynamic_cast<CRoadway*>(r_element);
            if(roadway != nullptr && turn->get_horizontal_direction_possible() == EHorizontalMoveDirection::left
                && turn->get_vertical_direction_possible() == EVerticalMoveDirection::down){
                continue;
            }
        }
        ///

        return EMapElementPositionValidity::invalid;
    }

    return EMapElementPositionValidity::valid;
}

EMapElementPositionValidity CMapCreationController::get_road_element_position_validity_in_relation_to_surroundings(CRoadElement *r_element, QPoint placement_position)
{
    auto road_element_type = r_element->get_road_element_type();

    switch(road_element_type){
    case ERoadElementType::roadway_element:
        return get_roadway_element_position_validity_in_relation_to_surroundings(static_cast<CRoadwayElement*>(r_element), placement_position);
        break;
    case ERoadElementType::pavement:
        return get_pavement_position_validity_in_relation_to_surroundings(static_cast<CPavement*>(r_element), placement_position);
        break;
    case ERoadElementType::pedestrian_crossing:
        return get_pedestrian_crossing_position_validity_in_relation_to_surroundings(static_cast<CPedestrianCrossing*>(r_element), placement_position);
        break;
    default:
        return EMapElementPositionValidity::invalid;
    }
}

EMapElementPositionValidity CMapCreationController::get_pavement_position_validity_in_relation_to_surroundings(CPavement *pavement, QPoint placement_position)
{
    auto pavement_rect = pavement->boundingRect().toRect();
    auto pavement_width = pavement_rect.width();
    auto pavement_height = pavement_rect.height();

    bool pavement_is_in_adjacent_cell = false;
    QList<QList<QGraphicsItem*>> adjacent_cells_items_lists =
        get_elements_adjacent_cells_items(pavement_width, pavement_height, pavement->rotation(), placement_position);


    for(int i = 0; i < adjacent_cells_items_lists.size(); i++){
        if(pavement_is_in_adjacent_cell)
            break;
        if(adjacent_cells_items_lists[i].size() != 0){
            for(auto item : adjacent_cells_items_lists[i]){
                if(dynamic_cast<CPavement*>(item) != nullptr && item != pavement){
                    pavement_is_in_adjacent_cell = true;
                    break;
                }
            }
        }
    }

    if(pavement_is_in_adjacent_cell){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::initially_valid;
    }
}

EMapElementPositionValidity CMapCreationController::get_pedestrian_crossing_position_validity_in_relation_to_surroundings(CPedestrianCrossing *p_crossing, QPoint placement_position)
{
    auto p_crossing_movement_plane = p_crossing->get_movement_plane();
    auto p_crossing_rect = p_crossing->boundingRect().toRect();
    auto p_crossing_width = p_crossing_rect.width();
    auto p_crossing_height = p_crossing_rect.height();
    auto default_element_size = CStationaryMapElement::get_default_map_element_size();

    //Items bounding rect does not follow rotations
    if((p_crossing_width != p_crossing_height) && (int)p_crossing->rotation()%180 != 0)
    {
        int buff = p_crossing_width;
        p_crossing_width = p_crossing_height;
        p_crossing_height = buff;
    }

    int preceding_and_following_cells_to_check = ceil(p_crossing_width/default_element_size.width());
    int side_cells_to_check = ceil(p_crossing_height/default_element_size.height());
    QList<QList<QGraphicsItem*>> adjacent_cells_items_lists =
        get_elements_adjacent_cells_items(p_crossing_width, p_crossing_height, p_crossing->rotation(), placement_position);

    bool are_left_cells_valid = false;
    bool are_right_cells_valid = false;

    int i, j, k, l;
    if(p_crossing_movement_plane == EMovementPlane::vertical){
        i = 0; j = 2; k = 2; l = 4;
    }
    else{
        i = 2; j = 4; k = 0; l = 2;
    }
    int valid_cells_counter = 0;

    //Side cells validation
    for(;i < j; i++){
        if(adjacent_cells_items_lists[i].size() != 0){
            for(auto item : adjacent_cells_items_lists[i]){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element || road_element == p_crossing)
                    continue;
    
                //TODO
                auto road_element_type = road_element->get_road_element_type();
                if(road_element_type == ERoadElementType::pavement){
                    return EMapElementPositionValidity::invalid;
                }
    
                if(road_element_type == ERoadElementType::roadway_element ||
                    (road_element_type == ERoadElementType::pedestrian_crossing && road_element->get_movement_plane() == p_crossing_movement_plane)){
                    valid_cells_counter++;
                }
            }
        }
        if(valid_cells_counter == side_cells_to_check){
            if(i%2 == 0){
                are_left_cells_valid = true;
            }
            else{
                are_right_cells_valid = true;
            }
        }
        valid_cells_counter = 0;
    }

    bool are_preceding_cells_valid = false;
    bool are_following_cells_valid = false;

    for(;k < l; k++){
        if(adjacent_cells_items_lists[k].size() != 0){
            for(auto item : adjacent_cells_items_lists[k]){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element || road_element == p_crossing)
                    continue;

                auto road_element_type = road_element->get_road_element_type();
                if(road_element_type == ERoadElementType::roadway_element){
                    return EMapElementPositionValidity::invalid;
                }

                if((road_element_type == ERoadElementType::pedestrian_crossing && road_element->get_movement_plane() == p_crossing_movement_plane)
                    || road_element_type == ERoadElementType::pavement){
                    valid_cells_counter++;
                }
            }
        }
        if(valid_cells_counter == preceding_and_following_cells_to_check){
            if(k%2 == 0){
                are_preceding_cells_valid = true;
            }
            else{
                are_following_cells_valid = true;
            }
        }
        valid_cells_counter = 0;
    }

    if(are_preceding_cells_valid && are_following_cells_valid && are_left_cells_valid && are_right_cells_valid){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::initially_valid;
    }
}

EMapElementPositionValidity CMapCreationController::get_roadway_element_position_validity_in_relation_to_surroundings(CRoadwayElement *rw_element, QPoint placement_position)
{
    auto roadway_element_type = rw_element->get_roadway_element_type();
    switch(roadway_element_type){
    case ERoadwayElementType::roadway:
        return get_roadway_position_validity_in_relation_to_surroundings(static_cast<CRoadway*>(rw_element), placement_position);
        break;
    case ERoadwayElementType::crossing:
        return get_crossing_position_validity_in_relation_to_surroundings(static_cast<CCrossing*>(rw_element), placement_position);
        break;
    case ERoadwayElementType::turn:
        return get_turn_position_validity_in_relation_to_surroundings(static_cast<CTurn*>(rw_element), placement_position);
        break;
    default:
        return EMapElementPositionValidity::invalid;
        break;
    }
}

EMapElementPositionValidity CMapCreationController::get_crossing_position_validity_in_relation_to_surroundings(CCrossing *crossing, QPoint placement_position)
{
    auto crossing_rect = crossing->boundingRect().toRect();
    auto crossing_width = crossing_rect.width();
    auto crossing_height = crossing_rect.height();
    int crossing_carriageways_number = crossing->get_carriageways_number();
    int crossing_lanes_number = crossing->get_lanes_number();

    auto default_element_size = CStationaryMapElement::get_default_map_element_size();
    QList<QList<QGraphicsItem*>> adjacent_cells_items_lists =
        get_elements_adjacent_cells_items(crossing_width, crossing_height, crossing->rotation(), placement_position);

    int side_cells_to_check = ceil(crossing->pixmap().rect().height()/default_element_size.height());
    int preceding_and_following_cells_to_check = ceil(crossing->pixmap().rect().width()/default_element_size.width());

    bool are_left_cells_valid = false;
    bool are_right_cells_valid = false;
    int valid_cells_counter = 0;

    for(int i = 0; i < 2; i++){
        if(adjacent_cells_items_lists[i].size() != 0){
            for(auto item : adjacent_cells_items_lists[i]){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element || road_element == crossing)
                    continue;

                if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
                    if(road_element->get_movement_plane() == EMovementPlane::horizontal){
                        return EMapElementPositionValidity::invalid;
                    }
                    valid_cells_counter++;
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::roadway_element){
                    auto roadway_element = static_cast<CRoadwayElement*>(road_element);
                    auto roadway_element_type = roadway_element->get_roadway_element_type();
                    if(!(roadway_element_type == ERoadwayElementType::roadway && roadway_element->get_movement_plane() == EMovementPlane::horizontal &&
                          roadway_element->get_carriageways_number() == crossing_carriageways_number &&
                          roadway_element->get_lanes_number() == crossing_lanes_number)){
                        return EMapElementPositionValidity::invalid;
                    }

                    valid_cells_counter++;
                }
            }
        }
        if(valid_cells_counter == side_cells_to_check){
            if(i == 0){
                are_left_cells_valid = true;
            }
            else{
                are_right_cells_valid = true;
            }
        }
        valid_cells_counter = 0;
    }


    bool are_preceding_cells_valid = false;
    bool are_following_cells_valid = false;

    for(int i = 2; i < 4; i++){
        if(adjacent_cells_items_lists[i].size() != 0){
            for(auto item : adjacent_cells_items_lists[i]){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element || road_element == crossing)
                    continue;

                if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
                    if(road_element->get_movement_plane() == EMovementPlane::vertical){
                        return EMapElementPositionValidity::invalid;
                    }
                    valid_cells_counter++;
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::roadway_element){
                    auto roadway_element = static_cast<CRoadwayElement*>(road_element);
                    auto roadway_element_type = roadway_element->get_roadway_element_type();
                    if(!(roadway_element_type == ERoadwayElementType::roadway && roadway_element->get_movement_plane() == EMovementPlane::vertical &&
                          roadway_element->get_carriageways_number() == crossing_carriageways_number &&
                          roadway_element->get_lanes_number() == crossing_lanes_number)){
                        return EMapElementPositionValidity::invalid;
                    }

                    valid_cells_counter++;
                }
            }
        }
        if(valid_cells_counter == preceding_and_following_cells_to_check){
            if(i == 2){
                are_preceding_cells_valid = true;
            }
            else{
                are_following_cells_valid = true;
            }
        }
        valid_cells_counter = 0;
    }

    //At least three sides of a crossing need to be valid roadway continuations for it to be a valid crossing
    if(are_left_cells_valid + are_right_cells_valid + are_following_cells_valid + are_preceding_cells_valid >= 3){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::initially_valid;
    }
}

EMapElementPositionValidity CMapCreationController::get_turn_position_validity_in_relation_to_surroundings(CTurn *turn, QPoint placement_position)
{
    auto turn_rect = turn->boundingRect().toRect();
    auto turn_width = turn_rect.width();
    auto turn_height = turn_rect.height();
    auto default_element_size = CStationaryMapElement::get_default_map_element_size();

    QList<QList<QGraphicsItem*>> adjacent_cells_items_lists =
        get_elements_adjacent_cells_items(turn_width, turn_height, turn->rotation(), placement_position);

    //Items bounding rect does not follow rotations
    if((turn_width != turn_height) && (int)turn->rotation()%180 != 0)
    {
        turn_width = turn_height;
    }

    int preceding_and_following_cells_to_check = ceil(turn_width/default_element_size.width());

    QList<QList<QGraphicsItem*>> preceding_and_following_cells_items_lists;
    // QList<QList<QGraphicsItem*>> side_cells_items_lists;

    EVerticalMoveDirection turn_vertical_move_direction = turn->get_vertical_direction_possible();
    EHorizontalMoveDirection turn_horizontal_move_direction = turn->get_horizontal_direction_possible();
    if(turn_vertical_move_direction == EVerticalMoveDirection::up){
        if(turn_horizontal_move_direction == EHorizontalMoveDirection::left){
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[0]);
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[3]);
        }
        else{
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[2]);
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[1]);
        }
    }
    else{
        if(turn_horizontal_move_direction == EHorizontalMoveDirection::left){
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[3]);
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[1]);
        }
        else{
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[0]);
            preceding_and_following_cells_items_lists.append(adjacent_cells_items_lists[2]);
        }
    }

    int turn_carriageways_number = turn->get_carriageways_number();
    int turn_lanes_number = turn->get_lanes_number();

    int valid_cells_counter = 0;
    bool are_preceding_cells_valid = false;
    bool are_following_cells_valid = false;

    //Preceding/following cells validation
    for(int i = 0; i < 2; i++){
        bool item_is_in_preceding_cell = false;
        if(i == 0){
            item_is_in_preceding_cell = true;
        }
        if(preceding_and_following_cells_items_lists[i].size() != 0){
            for(auto item : preceding_and_following_cells_items_lists[i]){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element || road_element == turn)
                    continue;

                auto road_element_type = road_element->get_road_element_type();
                //Turn can have a dead end, it's not invalid, but it means the other side needs to be a proper roadway continuation
                if(road_element_type == ERoadElementType::pavement){
                    break;
                }

                if(road_element_type == ERoadElementType::pedestrian_crossing){
                        return EMapElementPositionValidity::invalid;
                }

                if(road_element_type == ERoadElementType::roadway_element){
                    auto roadway_element = static_cast<CRoadwayElement*>(road_element);
                    if(roadway_element->get_carriageways_number() != turn_carriageways_number ||
                        roadway_element->get_lanes_number() != turn_lanes_number)
                        return EMapElementPositionValidity::invalid;

                    if(roadway_element->get_roadway_element_type() == ERoadwayElementType::roadway){
                        if(item_is_in_preceding_cell){
                            if(roadway_element->get_movement_plane() == EMovementPlane::vertical){
                                if((turn_horizontal_move_direction == EHorizontalMoveDirection::right &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::down) ||
                                    (turn_horizontal_move_direction == EHorizontalMoveDirection::left &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::up)){
                                    return EMapElementPositionValidity::invalid;
                                }
                            }
                            else{
                                if((turn_horizontal_move_direction == EHorizontalMoveDirection::right &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::up) ||
                                    (turn_horizontal_move_direction == EHorizontalMoveDirection::left &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::down)){
                                    return EMapElementPositionValidity::invalid;
                                }
                            }
                        }
                        else{
                            if(roadway_element->get_movement_plane() == EMovementPlane::vertical){
                                if((turn_horizontal_move_direction == EHorizontalMoveDirection::right &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::up) ||
                                    (turn_horizontal_move_direction == EHorizontalMoveDirection::left &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::down)){
                                    return EMapElementPositionValidity::invalid;
                                }
                            }
                            else{
                                if((turn_horizontal_move_direction == EHorizontalMoveDirection::right &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::down) ||
                                    (turn_horizontal_move_direction == EHorizontalMoveDirection::left &&
                                     turn_vertical_move_direction == EVerticalMoveDirection::up)){
                                    return EMapElementPositionValidity::invalid;
                                }
                            }
                        }
                    }

                    if(roadway_element->get_roadway_element_type() == ERoadwayElementType::turn){
                        return EMapElementPositionValidity::invalid;
                    }
                }

                valid_cells_counter++;
            }
            if(valid_cells_counter == preceding_and_following_cells_to_check){
                if(item_is_in_preceding_cell){
                    are_preceding_cells_valid = true;
                }
                else{
                    are_following_cells_valid = true;
                }
            }
        }
    }

    //Turn needs to have valid continuation in at least one direction
    if(are_preceding_cells_valid || are_following_cells_valid){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::initially_valid;
    }

    return EMapElementPositionValidity::initially_valid;
}

EMapElementPositionValidity CMapCreationController::get_traffic_control_element_position_validity_in_relation_to_foundations(CTrafficControlElement *tc_element, const QList<QGraphicsItem *> &colliding_items)
{
    for(auto colliding_item : colliding_items){
        if(colliding_item == tc_element)
            continue;

        auto map_element = dynamic_cast<CStationaryMapElement*>(colliding_item);
        if(map_element == nullptr)
            continue;

        if(map_element->get_map_element_type() == EStationaryMapElementType::filler)
            continue;

        if(map_element->get_map_element_type() == EStationaryMapElementType::road_element){
            if(dynamic_cast<CPavement*>(map_element))
                continue;
        }

        return EMapElementPositionValidity::invalid;
    }

    return EMapElementPositionValidity::valid;
}

EMapElementPositionValidity CMapCreationController::get_traffic_control_element_position_validity_in_relation_to_surroundings(CStationaryMapElement *tc_element, QPoint placement_position)
{
    auto default_cell_size = CReadOnlyMap::get_default_cell_size();
    auto left_side_cell_center_pos = QPoint(placement_position.x() - default_cell_size.width()/2,
                                            placement_position.y() + default_cell_size.height()/2);
    auto right_side_cell_center_pos = QPoint(placement_position.x() + 1.5*default_cell_size.width(),
                                             placement_position.y() + default_cell_size.height()/2);

    QList<QList<QGraphicsItem*>> side_cells_items;
    side_cells_items.append(m_map_model->items(left_side_cell_center_pos));
    side_cells_items.append(m_map_model->items(right_side_cell_center_pos));
    int invalid_sides_counter = 0;

    //Roadway at least on one side of the traffic control element is required
    for(int i = 0; i < 2; i ++){
        if(side_cells_items[i].size() != 0){
            for(auto item : side_cells_items[i]){
                if(item == tc_element)
                    continue;

                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element)
                    continue;

                if(road_element->get_road_element_type() == ERoadElementType::pavement){
                    invalid_sides_counter++;
                    break;
                }

                if(road_element->get_road_element_type() == ERoadElementType::roadway_element || road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
                    return EMapElementPositionValidity::valid;
                }
            }
        }
    }

    if(invalid_sides_counter == 2){
        return EMapElementPositionValidity::invalid;
    }

    return EMapElementPositionValidity::initially_valid;
}

EMapElementPositionValidity CMapCreationController::get_element_position_validity(CStationaryMapElement *element, QPoint placement_position)
{
    auto *dummy_element = new QGraphicsPixmapItem();
    dummy_element->setPixmap(element->pixmap());
    dummy_element->setVisible(true);
    dummy_element->setPos(placement_position);
    if(element->rotation() != 0){
        dummy_element->setTransformOriginPoint(dummy_element->boundingRect().center());
        dummy_element->setRotation(element->rotation());
    }
    m_map_model->addItem(dummy_element);
    auto colliding_items = m_map_model->collidingItems(dummy_element);

    auto element_pos_validity = get_element_position_validity_in_relation_to_foundations(element, colliding_items);
    if(element_pos_validity != EMapElementPositionValidity::invalid){
        element_pos_validity = get_element_position_validity_in_relation_to_surroundings(element, placement_position);
    }

    m_map_model->removeItem(dummy_element);
    delete dummy_element;

    return element_pos_validity;
}

EMapElementPositionValidity CMapCreationController::get_element_position_validity_in_relation_to_foundations(CStationaryMapElement *element, const QList<QGraphicsItem *> &colliding_items)
{
    auto map_element_type = element->get_map_element_type();
    switch (map_element_type){
    case EStationaryMapElementType::traffic_control_element:
        return get_traffic_control_element_position_validity_in_relation_to_foundations(static_cast<CTrafficControlElement*>(m_element_being_placed), colliding_items);
        break;
    case EStationaryMapElementType::road_element:
        return get_road_element_position_validity_in_relation_to_foundations(static_cast<CRoadElement*>(m_element_being_placed), colliding_items);
        break;
    default:
        return EMapElementPositionValidity::invalid;
    }
}

EMapElementPositionValidity CMapCreationController::get_element_position_validity_in_relation_to_surroundings(CStationaryMapElement *element, QPoint placement_position)
{
    auto map_element_type = element->get_map_element_type();
    switch (map_element_type){
    case EStationaryMapElementType::traffic_control_element:
        return get_traffic_control_element_position_validity_in_relation_to_surroundings(static_cast<CTrafficControlElement*>(m_element_being_placed), placement_position);
        break;
    case EStationaryMapElementType::road_element:
        return get_road_element_position_validity_in_relation_to_surroundings(static_cast<CRoadElement*>(m_element_being_placed), placement_position);
        break;
    default:
        return EMapElementPositionValidity::invalid;
    }
}

EMapElementPositionValidity CMapCreationController::get_element_position_validity_in_relation_to_surroundings_by_serialization(CStationaryMapElement *element, QPoint placement_position)
{
    QString type_serialized = element->serialize_type_as_string();
    QStringList type_splitted = type_serialized.split("_");
    type_splitted.removeFirst();

    EMapElementPositionValidity position_validity = EMapElementPositionValidity::invalid;

    switch(type_splitted[0].toInt()){
    case EStationaryMapElementType::traffic_control_element:
        position_validity =
            get_traffic_control_element_position_validity_in_relation_to_surroundings(static_cast<CTrafficControlElement*>(element), placement_position);
        break;
    case EStationaryMapElementType::road_element:
        switch(type_splitted[1].toInt()){
        case ERoadElementType::pavement:
            position_validity =
                get_pavement_position_validity_in_relation_to_surroundings(static_cast<CPavement*>(element), placement_position);
            break;
        case ERoadElementType::pedestrian_crossing:
            position_validity =
                get_pedestrian_crossing_position_validity_in_relation_to_surroundings(static_cast<CPedestrianCrossing*>(element), placement_position);
            break;
        case ERoadElementType::roadway_element:
            switch(type_splitted[2].toInt()){
            case ERoadwayElementType::roadway:
                position_validity = get_roadway_position_validity_in_relation_to_surroundings(static_cast<CRoadway*>(element), placement_position);
                break;
            case ERoadwayElementType::crossing:
                position_validity = get_crossing_position_validity_in_relation_to_surroundings(static_cast<CCrossing*>(element), placement_position);
                break;
            case ERoadwayElementType::turn:
                position_validity = get_turn_position_validity_in_relation_to_surroundings(static_cast<CTurn*>(element), placement_position);
                break;
            }
        }
    }

    return position_validity;
}

bool CMapCreationController::is_item_movable(CStationaryMapElement *item)
{
    bool item_is_filler = dynamic_cast<CFiller*>(item) != nullptr;
    if(item == nullptr || item_is_filler)
        return false;
    return true;
}

void CMapCreationController::rotate_element_being_placed(QWheelEvent *event)
{
    int rotation_delta = event->angleDelta().y() / 90;
    qreal current_rotation = (int)m_element_being_placed->rotation() % 360;
    int new_rotation = qRound(current_rotation / 90) * 90 + rotation_delta * 90;

    auto *road_element = dynamic_cast<CRoadElement*>(m_element_being_placed);
    if(road_element != nullptr){
        if(road_element->get_movement_plane() == EMovementPlane::horizontal){
            road_element->set_movement_plane(EMovementPlane::vertical);
        }
        else{
            road_element->set_movement_plane(EMovementPlane::horizontal);
        }
    }

    auto *turn = dynamic_cast<CTurn*>(road_element);
    // TODO: use general rule for setting turns movement directions based on current rotation
    if(turn != nullptr){
        if(turn ->get_vertical_direction_possible() == EVerticalMoveDirection::down){
            turn ->set_vertical_direction_possible(EVerticalMoveDirection::up);
        }
        else
        {
            turn ->set_vertical_direction_possible(EVerticalMoveDirection::down);
        }

        if(new_rotation == 90 || abs(new_rotation) == 180 || new_rotation == -270){
            turn ->set_horizontal_direction_possible(EHorizontalMoveDirection::left);
        }
        else{
            turn ->set_horizontal_direction_possible(EHorizontalMoveDirection::right);
        }
    }

    QPointF center = m_element_being_placed->boundingRect().center();
    m_element_being_placed->setTransformOriginPoint(center);
    m_element_being_placed->setRotation(new_rotation);

    m_validation_rect->setTransformOriginPoint(m_validation_rect->boundingRect().center());
    m_validation_rect->setRotation(new_rotation);
}

void CMapCreationController::erase_selected_element(QMouseEvent *event)
{
    if(m_element_is_being_placed){
        m_element_is_being_placed = false;
        m_map_model->erase_stationary_map_element(m_element_being_placed);
        m_map_model->erase_item(m_validation_rect);
    }
    else{
        auto *item_at_mouse_pos = m_map_model->itemAt(this->m_map_view->mapToScene(event->pos()), this->m_map_view->transform());
        auto *map_element = dynamic_cast<CStationaryMapElement*>(item_at_mouse_pos);

        if(map_element == nullptr){
            auto final_validation_rect = dynamic_cast<QGraphicsRectItem*>(item_at_mouse_pos);

            if(final_validation_rect != nullptr){
                auto element = m_elemenets_mapped_to_validation_rects.key(final_validation_rect);
                m_elemenets_mapped_to_validation_rects.remove(element);
                map_element = element;
                m_map_model->erase_item(final_validation_rect);
            }
            else{
                return;
            }
        }
        else if(!is_item_movable(map_element)){
            return;
        }

        m_map_model->erase_stationary_map_element(map_element);
    }
}

QPoint CMapCreationController::get_element_placement_position_snapped_to_grid(CStationaryMapElement *element)
{
    QSizeF default_map_element_size = CStationaryMapElement::get_default_map_element_size();
    QRect element_bounding_rect = element->boundingRect().toRect();
    int element_width = element_bounding_rect.width();
    int element_height = element_bounding_rect.height();

    int x_offset = 0;
    int y_offset = 0;

    if(abs((int)element->rotation()) % 180 == 90){
        x_offset = abs(element_width - element_height)/2;
        y_offset = abs(element_width - element_height)/2;
    }

    int x_pos_rounded_down_to_grid = (int)((element->pos().x() + element_width/2)/(int)default_map_element_size.width())*default_map_element_size.width();
    int y_pos_rounded_down_to_grid = (int)((element->pos().y() + element_height/2)/(int)default_map_element_size.height())*default_map_element_size.height();

    int new_x_pos = x_pos_rounded_down_to_grid - x_offset;
    int new_y_pos = y_pos_rounded_down_to_grid - y_offset;

    return QPoint(new_x_pos, new_y_pos);
}

QList<QList<QGraphicsItem*>> CMapCreationController::get_elements_adjacent_cells_items(int element_width, int element_height, double element_rotation, QPoint element_pos)
{
    //Items bounding rect does not follow rotations
    if((element_width != element_height) && (int)element_rotation%180 != 0)
    {
        int buff = element_width;
        element_width = element_height;
        element_height = buff;
    }

    QList<QList<QGraphicsItem*>> adjacent_cells_items;
    QList<QGraphicsItem*> left_cells_items, right_cells_items, lower_cells_items, upper_cells_items;
    auto default_element_size = CStationaryMapElement::get_default_map_element_size();
    int side_cells_to_check = ceil(element_height/default_element_size.height());
    int upper_and_lower_cells_to_check = ceil(element_width/default_element_size.width());

    for(int i = 0; i < side_cells_to_check; i++){
        QPoint left_cell_center_pos = QPoint(element_pos.x() - default_element_size.width()/2,
                                             element_pos.y() + default_element_size.height()/2 + i*default_element_size.height());
        QPoint right_cell_center_pos = QPoint(element_pos.x() + element_width + default_element_size.width()/2,
                                              element_pos.y() + default_element_size.height()/2 + i*default_element_size.height());

        left_cells_items += m_map_model->items(left_cell_center_pos);
        right_cells_items += m_map_model->items(right_cell_center_pos);
    }

    for(int i = 0; i < upper_and_lower_cells_to_check; i++){
        QPoint preceding_cell_center_pos = QPoint(element_pos.x() + default_element_size.width()/2 + i*default_element_size.width(),
                                                  element_pos.y() + element_height + default_element_size.height()/2);
        QPoint following_cell_center_pos = QPoint(element_pos.x() + default_element_size.width()/2 + i*default_element_size.width(),
                                                  element_pos.y() - default_element_size.height()/2);

        lower_cells_items += m_map_model->items(preceding_cell_center_pos);
        upper_cells_items += m_map_model->items(following_cell_center_pos);
    }

    adjacent_cells_items.append(left_cells_items);adjacent_cells_items.append(right_cells_items);
    adjacent_cells_items.append(lower_cells_items);adjacent_cells_items.append(upper_cells_items);

    return adjacent_cells_items;
}

EMapElementPositionValidity CMapCreationController::get_roadway_position_validity_in_relation_to_surroundings(CRoadway *roadway, QPoint placement_position)
{
    auto roadway_movement_plane = roadway->get_movement_plane();
    auto rodaway_test = roadway->boundingRect();
    auto roadway_rect = rodaway_test.toRect();
    auto roadway_width = roadway_rect.width();
    auto roadway_height = roadway_rect.height();
    auto default_element_size = CStationaryMapElement::get_default_map_element_size();

    int x_offset = 0;
    int y_offset = 0;

    if(abs((int)roadway->rotation()) % 180 == 90 && roadway_height != roadway_width){
        x_offset = abs(roadway_width - roadway_height)/2;
        y_offset = abs(roadway_width - roadway_height)/2;
    }

    QPoint real_left_corner_pos = QPoint(placement_position.x() + x_offset, placement_position.y()-y_offset);

    QList<QList<QGraphicsItem*>> adjacent_cells_items_lists =
        get_elements_adjacent_cells_items(roadway_width, roadway_height, roadway->rotation(), real_left_corner_pos);

    int preceding_and_following_cells_to_check = ceil(roadway_width/default_element_size.width());

    int i, j, k, l;
    if(roadway_movement_plane == EMovementPlane::vertical){
        i = 0; j = 2; k = 2; l = 4;
    }
    else{
        i = 2; j = 4; k = 0; l = 2;
    }

    //Side cells validation
    for(;i < j; i++){
        if(adjacent_cells_items_lists[i].size() != 0){
            for(auto item : adjacent_cells_items_lists[i]){
                if(get_roadway_side_cell_item_validity(roadway, item) == EMapElementPositionValidity::invalid){
                    return EMapElementPositionValidity::invalid;
                }
            }
        }
    }

    int valid_cells_counter = 0;
    bool are_preceding_cells_valid = false;
    bool are_following_cells_valid = false;

    //Preceding/following cells validation
    for(; k < l; k++){
        bool item_is_in_preceding_cell = false;
        if(k%2 == 0){
            item_is_in_preceding_cell = true;
        }
        if(adjacent_cells_items_lists[k].size() != 0){
            for(auto item : adjacent_cells_items_lists[k]){

                EMapElementPositionValidity item_validity =
                    get_roadway_preceding_or_following_cell_item_validity(roadway, item, item_is_in_preceding_cell);

                if(item_validity == EMapElementPositionValidity::invalid){
                    return item_validity;
                }
                else if(item_validity == EMapElementPositionValidity::initially_valid){
                    continue;
                }
                else{
                    valid_cells_counter++;
                }
            }
        }
        if(valid_cells_counter == preceding_and_following_cells_to_check){
            if(item_is_in_preceding_cell){
                are_preceding_cells_valid = true;
            }
            else{
                are_following_cells_valid = true;
            }
        }
        valid_cells_counter = 0;
    }

    //Roadway needs to have valid continuation in at least one direction
    if(are_preceding_cells_valid || are_following_cells_valid){
        return EMapElementPositionValidity::valid;
    }
    else{
        return EMapElementPositionValidity::initially_valid;
    }
}

EMapElementPositionValidity CMapCreationController::get_roadway_preceding_or_following_cell_item_validity(CRoadway *roadway, QGraphicsItem *item, bool item_is_in_preceding_cell)
{
    auto road_element = dynamic_cast<CRoadElement*>(item);
    if(!road_element || road_element == roadway)
        return EMapElementPositionValidity::initially_valid;

    auto road_element_type = road_element->get_road_element_type();
    //Roadway can have a dead end, it's not invalid, but it means the other side needs to be a proper roadway continuation
    if(road_element_type == ERoadElementType::pavement){
        return EMapElementPositionValidity::initially_valid;
    }

    EMovementPlane roadway_movement_plane = roadway->get_movement_plane();
    if(road_element_type == ERoadElementType::pedestrian_crossing){
        //Pedestrian crossing should be perpendicular to roadway
        if(road_element->get_movement_plane() == roadway_movement_plane){
            return EMapElementPositionValidity::invalid;
        }

        return EMapElementPositionValidity::valid;
    }

    if(road_element_type == ERoadElementType::roadway_element){
        auto roadway_element = static_cast<CRoadwayElement*>(road_element);
        if(roadway_element->get_carriageways_number() != roadway->get_carriageways_number() ||
            roadway_element->get_lanes_number() != roadway->get_lanes_number()){
            return EMapElementPositionValidity::invalid;
        }

        if(roadway_element->get_roadway_element_type() == ERoadwayElementType::roadway && roadway_element->get_movement_plane() != roadway_movement_plane){
            return EMapElementPositionValidity::invalid;
        }

        if(roadway_element->get_roadway_element_type() == ERoadwayElementType::turn){
            auto turn_element = static_cast<CTurn*>(roadway_element);
            auto turn_horizontal_direction = turn_element->get_horizontal_direction_possible();
            auto turn_vertical_direction = turn_element->get_vertical_direction_possible();

            if(item_is_in_preceding_cell){
                if(roadway_movement_plane == EMovementPlane::horizontal &&
                    ((turn_vertical_direction == EVerticalMoveDirection::down && turn_horizontal_direction == EHorizontalMoveDirection::right) ||
                     (turn_vertical_direction == EVerticalMoveDirection::up && turn_horizontal_direction == EHorizontalMoveDirection::left)))
                {
                    return EMapElementPositionValidity::invalid;
                }

                if(roadway_movement_plane == EMovementPlane::vertical &&
                    ((turn_vertical_direction == EVerticalMoveDirection::up && turn_horizontal_direction == EHorizontalMoveDirection::right) ||
                     (turn_vertical_direction == EVerticalMoveDirection::down && turn_horizontal_direction == EHorizontalMoveDirection::right)))
                {
                    return EMapElementPositionValidity::invalid;
                }
            }
            else{
                if(roadway_movement_plane == EMovementPlane::horizontal &&
                    ((turn_vertical_direction == EVerticalMoveDirection::up && turn_horizontal_direction == EHorizontalMoveDirection::right) ||
                     (turn_vertical_direction == EVerticalMoveDirection::down && turn_horizontal_direction == EHorizontalMoveDirection::left)))
                {
                    return EMapElementPositionValidity::invalid;
                }

                if(roadway_movement_plane == EMovementPlane::vertical &&
                    ((turn_vertical_direction == EVerticalMoveDirection::up && turn_horizontal_direction == EHorizontalMoveDirection::left) ||
                     (turn_vertical_direction == EVerticalMoveDirection::down && turn_horizontal_direction == EHorizontalMoveDirection::left)))
                {
                    return EMapElementPositionValidity::invalid;
                }
            }
        }
    }
    return EMapElementPositionValidity::valid;
}

EMapElementPositionValidity CMapCreationController::get_roadway_side_cell_item_validity(CRoadway *roadway, QGraphicsItem *item)
{
    auto road_element = dynamic_cast<CRoadElement*>(item);
    if(!road_element || road_element == roadway)
        return EMapElementPositionValidity::valid;

    if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
        return EMapElementPositionValidity::invalid;
    }

    if(road_element->get_road_element_type() == ERoadElementType::roadway_element){
        auto roadway_element = static_cast<CRoadwayElement*>(road_element);
        auto roadway_element_type = roadway_element->get_roadway_element_type();
        if((roadway_element_type == ERoadwayElementType::roadway &&
             roadway_element->get_movement_plane() != roadway->get_movement_plane())
            || roadway_element_type == ERoadwayElementType::turn){
            return EMapElementPositionValidity::invalid;
        }
    }

    return EMapElementPositionValidity::valid;
}
