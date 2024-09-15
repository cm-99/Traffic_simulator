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

    /**
     * @brief Resizes the map's scene to given dimensions.
     * @param scene_width - new scene width
     * @param scene_height - new scene height
     */
    void resize_map(int scene_width, int scene_height);
    /**
     * @brief Fills map cells with graphical items of CFiller type.
     * If more representations of filler will be added it should randomly change those.
     */
    void fill_map();
    /**
     * @brief Adds guide grids separating map cells.
     */
    void add_guide_grid();
    /**
     * @brief Adds map boundaries in the form of graphical lines.
     */
    void add_map_boundaries();
    /**
     * @brief Removes all items of type CStationaryMapElement and deletes them.
     */
    void clear_stationary_map_elements();
    /**
     * @brief Adds road used at the specified position.
     * @param new_road_user - road user to be added
     * @param pos - position of the road user
     */
    void add_road_user(CRoadUser *new_road_user, QPointF pos);
    /**
     * @brief Adds stationary map element at the specified positions. Checks if the element is of the CTrafficLight type,
     * if it is, adds it to the corresponding list.
     * @param new_stationary_map_element - map element to be added
     * @param pos - position of the new element
     */
    void add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos);
    /**
     * @brief Removes road user from the map and deletes the object.
     * @param road_user_to_remove - road user to be erased
     */
    void erase_road_user(CRoadUser *road_user_to_remove);
    /**
     * @brief Removes from the map and deletes all objects of CRoadUser type.
     */
    void erase_road_users();
    /**
     * Removes item from the map and deletes it.
     * @param item
     */
    void erase_item(QGraphicsItem *item);
    /**
     * @brief Removes stationary map element from the map and deletes it. Checks if it is of the CTrafficLight type, if it is,
     * removes the pointer from corresponding list.
     * @param item - stationary map element to erase
     */
    void erase_stationary_map_element(CStationaryMapElement *item);

private:
    void erase_map_boundaries();
    void erase_guide_grid();
    void enlarge_map(int scene_width, int scene_height);
    void shrink_map(int scene_width, int scene_height);
};

#endif // CEDITABLEMAP_H
