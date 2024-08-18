#include "csimulationcontroller.h"
#include "MapViews/csimulationview.h"
#include <QMouseEvent>

CSimulationController::CSimulationController(CApplicationController *application_controller, CEditableMap *map_model, SSimulationConfiguration simulation_configuration) :
    CBaseController(application_controller),
    m_simulation_configuration(simulation_configuration)
{
    m_map_model = map_model;
    m_map_view = new CSimulationView(m_map_model, this);
}

CSimulationController::~CSimulationController()
{
    if(m_map_model != nullptr){
        delete m_map_model;
    }

    if(m_map_view != nullptr){
        delete m_map_view;
    }
}

void CSimulationController::set_user_to_place_creation_func(CRoadUser *(*creation_func)())
{
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
    if(!m_road_user_is_being_placed)
        return;

    QPointF mouse_event_mapped_to_scene = this->m_map_view->mapToScene(event->pos());
    QRectF element_bounding_rect = m_road_user_being_placed->boundingRect();
    if(m_road_user_is_being_placed)
    {
        m_road_user_being_placed -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2,
                                       mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2);
        m_validation_rect -> setPos(mouse_event_mapped_to_scene.x() - element_bounding_rect.width()/2 - m_val_rect_size_offset/2,
                                  mouse_event_mapped_to_scene.y() - element_bounding_rect.height()/2 - m_val_rect_size_offset/2);
        update_validation_rect();
    }
}

void CSimulationController::process_mouse_press_event(QMouseEvent *event)
{

}

void CSimulationController::start_simulation()
{

}

void CSimulationController::pause_simulation()
{

}

void CSimulationController::restart_simulation()
{

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

EMapElementPositionValidity CSimulationController::get_current_user_position_validity()
{
    auto colliding_items = m_map_model->collidingItems(m_road_user_being_placed);

    if(m_road_user_being_placed->get_road_user_type() == ERoadUsers::car){
        for(auto colliding_item : colliding_items){

            if(colliding_item == m_validation_rect){
                continue;
            }

            auto map_element = dynamic_cast<CStationaryMapElement*>(colliding_item);
            if(!map_element){
                return EMapElementPositionValidity::invalid;
            }

            if(map_element->get_map_element_type() == EStationaryMapElementType::filler){
                continue;
            }

            CRoadElement *road_element;
            if(map_element->get_map_element_type() == EStationaryMapElementType::road_element){
                road_element = static_cast<CRoadElement*>(map_element);
            }
            else{
                return EMapElementPositionValidity::invalid;
            }

            if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){
                if(road_element->get_movement_plane() == m_road_user_being_placed->get_starting_movement_plane()){
                    return EMapElementPositionValidity::invalid;
                }
                else{
                    continue;
                }
            }

            if(road_element->get_road_element_type() != ERoadElementType::roadway_element){
                return EMapElementPositionValidity::invalid;
            }
            else{
                auto roadway_element = static_cast<CRoadwayElement*>(road_element);
                if(roadway_element->get_roadway_element_type() != ERoadwayElementType::roadway
                    || roadway_element->get_movement_plane() != m_road_user_being_placed->get_starting_movement_plane()){
                    return EMapElementPositionValidity::invalid;
                }
            }
        }
    }
    else{
        return EMapElementPositionValidity::invalid;
    }

    return EMapElementPositionValidity::valid;
}

void CSimulationController::update_validation_rect()
{
    auto pos_validity = get_current_user_position_validity();

    if(pos_validity == EMapElementPositionValidity::valid){
        m_validation_rect -> setBrush(Qt::green);
    }
    else{
        m_validation_rect -> setBrush(Qt::red);
    }
}

void CSimulationController::rotate_road_user_being_placed(QWheelEvent *event)
{

}
