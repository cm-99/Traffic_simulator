#include "ceditablemap.h"
#include "MapElements/StationaryMapElements/cfiller.h"

CEditableMap::CEditableMap(int scene_width, int scene_height):
    CReadOnlyMap(scene_width, scene_height)
{}

void CEditableMap::fill_map()
{
    QSize default_map_element_size = CStationaryMapElement::get_default_map_element_size();
    int map_fillers_rows = this->height()/default_map_element_size.height();
    int map_fillers_columns = this->width()/default_map_element_size.width();

    for(int i = 0; i < map_fillers_rows; i++){
        for(int j = 0; j < map_fillers_columns; j++){
            auto *filler = new CFiller();
            this->add_stationary_map_element(filler, QPointF(j * default_map_element_size.width(), i * default_map_element_size.height()));
        }
    }
}

void CEditableMap::add_map_boundaries()
{
    this->addLine(0, 1, this->width(), 1);
    this->addLine(0, this->height() - 1, this->width(), this->height() - 1);

    this->addLine(0, 0, 0, this->height());
    this->addLine(this->width(), 0, this->width(), this->height());
}

void CEditableMap::clear_stationary_map_elements()
{
    for(auto element : *m_stationary_map_elements){
        removeItem(element);
        delete element;
    }

    m_stationary_map_elements->clear();
    m_traffic_lights->clear();
}

void CEditableMap::add_road_user(CRoadUser *new_road_user, QPointF pos)
{
    new_road_user->setPos(pos);
    m_road_users->append(new_road_user);
    addItem(new_road_user);
}

void CEditableMap::add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos)
{
    new_stationary_map_element->setPos(pos);
    //Omit fillers, waste of space
    if(new_stationary_map_element->get_map_element_type() != EStationaryMapElementType::filler){
        m_stationary_map_elements->append(new_stationary_map_element);
    }
    addItem(new_stationary_map_element);

    auto *traffic_light = dynamic_cast<CTrafficLight*>(new_stationary_map_element);
    if(traffic_light != nullptr){
        m_traffic_lights->append(traffic_light);
    }
}

void CEditableMap::erase_road_user(CRoadUser *road_user_to_remove)
{
    m_road_users->remove(m_road_users->indexOf(road_user_to_remove));
    removeItem(road_user_to_remove);
    delete road_user_to_remove;
}

void CEditableMap::erase_item(QGraphicsItem *item)
{
    removeItem(item);
    delete item;
}

void CEditableMap::erase_stationary_map_element(CStationaryMapElement *item)
{
    m_stationary_map_elements->removeAt(m_stationary_map_elements->indexOf(item));

    auto *traffic_light = dynamic_cast<CTrafficLight*>(item);
    if(traffic_light != nullptr){
        m_traffic_lights->remove(m_traffic_lights->indexOf(traffic_light));
    }

    removeItem(item);
    delete item;
}
