#include "ceditablemap.h"

CEditableMap::CEditableMap() {}

void CEditableMap::add_road_user(CRoadUser *new_road_user, QPointF pos)
{
    new_road_user->setPos(pos);
    m_road_users->append(new_road_user);
    addItem(new_road_user);
}

void CEditableMap::add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos)
{
    new_stationary_map_element->setPos(pos);
    m_stationary_map_elements->append(new_stationary_map_element);
    addItem(new_stationary_map_element);
}

void CEditableMap::add_traffic_light(CTrafficLight *new_traffic_light, QPointF pos)
{
    new_traffic_light->setPos(pos);
    m_traffic_lights->append(new_traffic_light);
    addItem(new_traffic_light);
}

void CEditableMap::remove_road_user(CRoadUser *road_user_to_remove)
{
    m_road_users->remove(m_road_users->indexOf(road_user_to_remove));
    removeItem(road_user_to_remove);
}

void CEditableMap::remove_stationary_map_element(CStationaryMapElement *stationary_map_element_to_remove)
{
    m_stationary_map_elements->remove(m_stationary_map_elements->indexOf(stationary_map_element_to_remove));
    removeItem(stationary_map_element_to_remove);
}

void CEditableMap::remove_traffic_light(CTrafficLight *traffic_light_to_remove)
{
    m_traffic_lights->remove(m_traffic_lights->indexOf(traffic_light_to_remove));
    removeItem(traffic_light_to_remove);
}
