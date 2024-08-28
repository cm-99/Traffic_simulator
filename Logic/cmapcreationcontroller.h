#ifndef CMAPCREATIONCONTROLLER_H
#define CMAPCREATIONCONTROLLER_H

#include "Logic/MapImportAndExportManagers/imapimportandexportmanager.h"
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
    explicit CMapCreationController(CApplicationController *application_controller);
    ~CMapCreationController();

    /**
     * @brief Processes wheel event. If map element is being placed and it is rotable, it will be rotated according to the wheel event.
     * Otherwise relays the event to corresponding view.
     * @param event
     * @return returns true if event was handled; otherwise returns false
     */
    bool process_wheel_event(QWheelEvent *event);
    /**
     * @brief Processes mouse move event. If the map element is being placed, it follows the cursor.
     * @param event
     */
    void process_mouse_move_event(QMouseEvent *event);
    /**
     * @brief Processes mouse press event. Depending on the button pressed and modifiers used it:
     * - (element is being placed + left button) places the element being placed (if position is valid)
     * - (element is being placed + right button) removes the element being placed
     * - (element is NOT being placed + left button) picks up the element at mouse position (if it can be picked up)
     * - (element is NOT being placed + right button) removes the element at mouse position (if it can be removed)
     * @param event
     */
    void process_mouse_press_event(QMouseEvent *event);

    const inline QList<QString> &get_supported_map_file_formats() const {return m_supported_map_file_formats;}
    /**
     * @brief Loads map from file provided as the argument.
     * @param map_file_path
     * @return if successful returns loaded map; otherwise, if file format is not supported or the contents are wrong, returns nullptr
     */
    CEditableMap *load_map_from_file(QString map_file_path);

    /**
     * @brief Loads map from file provided as the argument and takes ownership of it.
     * @param map_file_path
     * @return
     */
    bool load_map_into_creator(QString map_file_path);
    /**
     * @brief Delegates map saving to implementation of IMapImportAndExportManager which supports the file format of provided fil.
     * @param map_file_path
     * @return
     */
    bool delegate_map_saving(QString map_file_path);
    /**
     * @brief Removes all map elements from currently owned map.
     */
    void clear_map();
    /**
     * @brief Relays simulation start request to the main application controller with copy of currently owned map.
     */
    void process_simulation_start_request();
    /**
     * @brief Prepares map for simulation by filling in missing lights and connecting them and setting valid sides of crossings.
     * @param map_model - map to be prepared for simulation
     */
    void prepare_map_for_simulation(CEditableMap *map_model);
    /**
     * @brief Sets provided function pointer as the creation function which is to be used to create next elements to place.
     */
    void set_element_to_place_creation_func(CStationaryMapElement *(*creation_func)(void));

private:
    QList<QString> m_supported_map_file_formats;
    QMap<QString, IMapImportAndExportManager*> m_map_formats_mapped_to_managers;
    QMap<CStationaryMapElement*, QGraphicsRectItem*> m_elemenets_mapped_to_validation_rects;

    CStationaryMapElement *(*m_current_creation_func)(void);
    CStationaryMapElement *m_element_being_placed{nullptr};
    bool m_element_is_being_placed{false};
    bool m_element_is_being_edited{false};

    QGraphicsRectItem *m_validation_rect{nullptr};
    int m_val_rect_size_offset{6};

    void add_guide_grid();
    bool perform_final_map_validation();
    /**
     * @brief fill_in_missing_lights - fills in the missing lights corresponding to pedestrian crossing or intersection
     * Map must be successfuly validated first
     */
    void fill_in_and_connect_lights(CEditableMap *map_model);
    void set_crossings_valid_sides(CEditableMap *map_model);

    void place_element_and_prepare_next(QMouseEvent *event);
    void rotate_element_being_placed(QWheelEvent *event);
    void erase_selected_element(QMouseEvent *event);

    void prepare_validation_rect();
    QGraphicsRectItem* prepare_final_validation_rect(CStationaryMapElement *item);
    void update_validation_rect();

    bool is_item_movable(CStationaryMapElement *item);
    QPoint get_element_placement_position_snapped_to_grid(CStationaryMapElement *element);

    QList<QList<QGraphicsItem*>> get_elements_adjacent_cells_items(int element_width, int element_height, double element_rotation, QPoint element_pos);

    //TODO: Check if it would make sense to move validation to corresponding map elements. What about recursive inclusion?
    EMapElementPositionValidity get_element_position_validity(CStationaryMapElement *element, QPoint placement_position);
    EMapElementPositionValidity get_element_position_validity_in_relation_to_foundations(CStationaryMapElement *element,
                                                                                                         const QList<QGraphicsItem *> &colliding_items);
    EMapElementPositionValidity get_element_position_validity_in_relation_to_surroundings(CStationaryMapElement *element, QPoint placement_position);
    EMapElementPositionValidity get_element_position_validity_in_relation_to_surroundings_by_serialization(CStationaryMapElement *element, QPoint placement_position);

    EMapElementPositionValidity get_traffic_control_element_position_validity_in_relation_to_foundations(CTrafficControlElement *tc_element,
                                                                                                         const QList<QGraphicsItem *> &colliding_items);
    EMapElementPositionValidity get_traffic_control_element_position_validity_in_relation_to_surroundings(CTrafficControlElement *tc_element, QPoint placement_position);
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
    QVector<std::pair<int, ERoadElementType>> get_traffic_control_elements_valid_sides_with_types(QPoint placement_position);
    /**
     * @brief is_crossing_light checks if a light at the placement_position is corresponding to a crossing.
     * "Corresponding crossing" means that the light is placed on the right side of a road which coms to an intersection.
     *
     * @param valid_sides
     * @param placement_position
     * @return returns true and light position in relation to the crossing if it is corresponding to one;
     * otherwise returns false and -1.
     */
    std::pair<bool, int> is_crossing_light(QVector<std::pair<int, ERoadElementType>> valid_sides, QPoint placement_position);
    void fill_in_and_connect_pedestrian_crossing_light(CEditableMap *map_model, CTrafficLight *tl, QVector<std::pair<int, ERoadElementType>> &valid_sides, QPoint &placement_position);
    void fill_in_and_connect_crossing_light(CEditableMap *map_model, CTrafficLight *tl, QPoint &placement_position, int crossing_placement);
    QVector<int> get_crossing_valid_sides(CCrossing *cr);
    QVector<int> get_crossing_valid_sides(CCrossing *cr, CEditableMap *map_model);
    CTrafficLight *add_corresponding_light(CEditableMap *map_model, QPointF corresponding_light_pos, CTrafficLight *tl);
    CTrafficLight *add_opposing_light(CEditableMap *map_model, QPointF opposing_light_pos, CTrafficLight *tl);
};

#endif // CMAPCREATIONCONTROLLER_H
