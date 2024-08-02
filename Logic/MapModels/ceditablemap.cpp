#include "ceditablemap.h"
#include "MapElements/StationaryMapElements/cfiller.h"

CEditableMap::CEditableMap() {}

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

void CEditableMap::add_road_user(CRoadUser *new_road_user, QPointF pos)
{
    new_road_user->setPos(pos);
    m_road_users->append(new_road_user);
    addItem(new_road_user);
}

void CEditableMap::add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos)
{
    new_stationary_map_element->setPos(pos);
    addItem(new_stationary_map_element);

    auto *traffic_light = dynamic_cast<CTrafficLight*>(new_stationary_map_element);
    if(traffic_light != nullptr){
        m_traffic_lights->append(traffic_light);
    }
}

void CEditableMap::add_traffic_light(CTrafficLight *new_traffic_light, QPointF pos)
{
    new_traffic_light->setPos(pos);
    m_traffic_lights->append(new_traffic_light);
    addItem(new_traffic_light);
}

void CEditableMap::erase_road_user(CRoadUser *road_user_to_remove)
{
    m_road_users->remove(m_road_users->indexOf(road_user_to_remove));
    removeItem(road_user_to_remove);
    delete road_user_to_remove;
}

void CEditableMap::erase_traffic_light(CTrafficLight *traffic_light_to_remove)
{
    m_traffic_lights->remove(m_traffic_lights->indexOf(traffic_light_to_remove));
    removeItem(traffic_light_to_remove);
    delete traffic_light_to_remove;
}

void CEditableMap::erase_item(QGraphicsItem *item)
{
    auto *road_user = dynamic_cast<CRoadUser*>(item);
    if(road_user != nullptr){
        erase_road_user(road_user);
        return;
    }

    auto *traffic_light = dynamic_cast<CTrafficLight*>(item);
    if(traffic_light != nullptr){
        erase_traffic_light(traffic_light);
        return;
    }

    removeItem(item);
    delete item;
}
