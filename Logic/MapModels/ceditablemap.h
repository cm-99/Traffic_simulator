#ifndef CEDITABLEMAP_H
#define CEDITABLEMAP_H

#include "creadonlymap.h"
#include "MapElements/croaduser.h"
#include "MapElements/cstationarymapelement.h"

/**
 * @brief The CEditableMap class is a map model that allows control over map elements.
 */
class CEditableMap : public CReadOnlyMap
{
public:
    CEditableMap();

    void add_road_user(CRoadUser *new_road_user, QPointF pos);
    void add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos);
    void add_traffic_light(CTrafficLight *new_traffic_light, QPointF pos);

    void remove_road_user(CRoadUser *road_user_to_remove);
    void remove_stationary_map_element(CStationaryMapElement *stationary_map_element_to_remove);
    void remove_traffic_light(CTrafficLight *traffic_light_to_remove);
};

#endif // CEDITABLEMAP_H
