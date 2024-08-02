#ifndef CMAPCREATIONCONTROLLER_H
#define CMAPCREATIONCONTROLLER_H

#include "MapElements/StationaryMapElements/RoadElements/ccrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/cturn.h"
#include "MapElements/StationaryMapElements/croadelement.h"
#include "MapElements/StationaryMapElements/RoadElements/croadway.h"
#include "cbasecontroller.h"

/**
 * @brief The EMapElementPositionValidity enum determines if elements placement is valid.
 * Created instead of simply using bool value to include a third possible element placement state - initially valid.
 * This state means that for the ease of map making some elements can be initially placed semi-correctly,
 * but their end state is also checked at the time of map saving/starting simulation.
 */
enum class EMapElementPositionValidity{valid, invalid, initially_valid};
/**
 * @brief The CMapCreationController class is a controller responsible for the map creation process.
 */
class CMapCreationController : public CBaseController
{
public:
    explicit CMapCreationController();
    CMapCreationController(CEditableMap *map_model);
    ~CMapCreationController();

    void set_element_to_place_creation_func(CStationaryMapElement *(*creation_func)(void));
    bool process_wheel_event(QWheelEvent *event);
    void process_mouse_move_event(QMouseEvent *event);
    void process_mouse_press_event(QMouseEvent *event);

private:
    CStationaryMapElement *(*m_current_creation_func)(void);
    CStationaryMapElement *m_element_being_placed{nullptr};
    bool m_element_is_being_placed{false};
    bool m_element_is_being_edited{false};

    QGraphicsRectItem *m_validation_rect{nullptr};
    int m_val_rect_size_offset{5};

    void add_guide_grid();

    void place_element_and_prepare_next(QMouseEvent *event);
    void rotate_element_being_placed(QWheelEvent *event);
    void erase_selected_element(QMouseEvent *event);

    void prepare_validation_rect();
    void update_validation_rect();

    bool is_item_movable(CStationaryMapElement *item);
    QPoint get_element_placement_position_snapped_to_grid(CStationaryMapElement *element);
    /**
     * For now - it returns left/right/lower/upper center positions
     * @brief get_elements_adjacent_cells_centers_positions
     * @param element_width
     * @param element_height
     * @return
     */
    QList<QList<QGraphicsItem*>> get_elements_adjacent_cells_items(int element_width, int element_height, double element_rotation, QPoint element_pos);

    //TODO: Check if it would make sense to move validation to corresponding map elements. What about recursive inclusion?
    EMapElementPositionValidity get_element_position_validity(CStationaryMapElement *element, QPoint placement_position);
    EMapElementPositionValidity get_element_position_validity_in_relation_to_foundations(CStationaryMapElement *element,
                                                                                                         const QList<QGraphicsItem *> &colliding_items);
    EMapElementPositionValidity get_element_position_validity_in_relation_to_surroundings(CStationaryMapElement *element, QPoint placement_position);

    EMapElementPositionValidity get_traffic_control_element_position_validity_in_relation_to_foundations(CTrafficControlElement *tc_element,
                                                                                                         const QList<QGraphicsItem *> &colliding_items);
    EMapElementPositionValidity get_traffic_control_element_position_validity_in_relation_to_surroundings(CStationaryMapElement *tc_element, QPoint placement_position);

    EMapElementPositionValidity get_road_element_position_validity_in_relation_to_foundations(CRoadElement *r_element, const QList<QGraphicsItem *> &colliding_items);
    EMapElementPositionValidity get_road_element_position_validity_in_relation_to_surroundings(CRoadElement *r_element, QPoint placement_position);

    EMapElementPositionValidity get_pavement_position_validity_in_relation_to_surroundings(CPavement *pavement, QPoint placement_position);
    EMapElementPositionValidity get_pedestrian_crossing_position_validity_in_relation_to_surroundings(CPedestrianCrossing *p_crossing, QPoint placement_position);

    EMapElementPositionValidity get_roadway_element_position_validity_in_relation_to_surroundings(CRoadwayElement *rw_element, QPoint placement_position);
    EMapElementPositionValidity get_roadway_position_validity_in_relation_to_surroundings(CRoadway *roadway, QPoint placement_position);
    EMapElementPositionValidity get_roadway_preceding_or_following_cell_item_validity(CRoadway *roadway, QGraphicsItem *item, bool item_is_in_preceding_cell);
    EMapElementPositionValidity get_roadway_side_cell_item_validity(CRoadway *roadway, QGraphicsItem *item);

    EMapElementPositionValidity get_crossing_position_validity_in_relation_to_surroundings(CCrossing *crossing, QPoint placement_position);
    EMapElementPositionValidity get_turn_position_validity_in_relation_to_surroundings(CTurn *turn, QPoint placement_position);
};

#endif // CMAPCREATIONCONTROLLER_H
