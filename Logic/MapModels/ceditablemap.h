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
    CEditableMap(int scene_width, int scene_height);

    void fill_map();
    void add_map_boundaries();
    void clear_stationary_map_elements();

    void add_road_user(CRoadUser *new_road_user, QPointF pos);
    void add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos);

    void erase_road_user(CRoadUser *road_user_to_remove);
    void erase_road_users();
    void erase_item(QGraphicsItem *item);
    void erase_stationary_map_element(CStationaryMapElement *item);
};

#endif // CEDITABLEMAP_H
