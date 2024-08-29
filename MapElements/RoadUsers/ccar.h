#ifndef CCAR_H
#define CCAR_H

#include "MapElements/StationaryMapElements/RoadElements/ccrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/croadway.h"
#include "MapElements/StationaryMapElements/RoadElements/cturn.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"
#include "MapElements/croaduser.h"

enum ECarMovementStates{riding, riding_to_pedestrian_crossing_with_lights, waiting_to_cross_pedestrian_crossing_w_lights,
                          riding_to_pedestrian_crossing_without_lights, waiting_to_cross_pedestrian_crossing_wi_lights,
                          crossing_p_crossing, turning_around, riding_to_turn, taking_turn, riding_to_crossing_with_lights,
                          riding_to_crossing_without_lights, waiting_to_cross_crossing_with_lights,
                          waiting_to_cross_crossing_without_lights, riding_through_crossing, taking_left_turn, taking_right_turn,
                          riding_straight_through_crossing, getting_closer_to_the_crossing};

/**
 * @brief The CCar class is a graphical and behavioral representation of a car.
 */
class CCar : public CRoadUser
{
public:
    static CRoadUser *create_car();

    virtual void move(CReadOnlyMap *map) override;
    bool is_braking() const {return m_is_braking;}
    bool is_waiting() const {return m_is_waiting;}
    bool is_turning_left() const {return m_is_turning_left;}
    bool is_turning_right() const {return m_is_turning_right;}
    void reset_state() override;

private:
    CCar(QPixmap car_pixmap, QString description, EMovementPlane movement_plane,
         EHorizontalMoveDirection horizontal_move_direction = EHorizontalMoveDirection::left,
         EVerticalMoveDirection vertical_move_direction = EVerticalMoveDirection::up);

    void set_destination(QPointF destination, std::vector<QPoint> path) override;
    void destination_reached();

    bool m_was_waiting_for_simulation_to_start{false};
    QPointF m_turn_end_point;
    double m_rotation_remaining{0};
    double m_arc_length_remaining{0};
    bool m_movement_planes_changed{false};
    bool m_turn_started{false};
    bool m_is_braking{false};
    bool m_is_waiting{false};
    bool m_crossing_after_p_crossing{false};
    bool m_was_leaving_a_crossing{false};
    bool m_is_turning_left{false};
    bool m_is_turning_right{false};
    bool m_there_is_road_priority_sign_to_check{false};
    bool m_lights_present_but_inactive{false};

    //What different signs boil down to
    QList<std::pair<EHorizontalMoveDirection, bool>> m_allowed_horizontal_movements
        {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};

    QList<std::pair<EVerticalMoveDirection, bool>> m_allowed_vertical_movements
        {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};

    std::pair<ETrafficSignCategory, int> m_current_road_priority_sign;
    int m_turn_width{0};
    int m_crossing_side_chosen{0};

    double m_riding_through_crossing_offset{100};
    int m_global_speed_limit{(50*10)/36}; //For now, city speed limit
    int m_temporary_speed_limit{-1};

    CPedestrianCrossing *m_p_crossing{nullptr};
    CTurn *m_turn{nullptr};
    CCrossing *m_crossing{nullptr};
    CTrafficLight *m_light{nullptr};
    CTrafficSign *m_last_traffic_sign{nullptr};

    ECarMovementStates m_current_movement_state{ECarMovementStates::riding};
    ECarMovementStates m_previous_movement_state{ECarMovementStates::riding};
    QMap<ECarMovementStates, void (CCar::*)()> m_movement_states_mapped_to_func;

    //Methods for random movement
    void ride();

    void ride_to_pedestrian_crossing_with_lights();
    void ride_to_pedestrian_crossing_without_lights();
    void wait_to_cross_pedestrian_crossing_with_lights();
    void wait_to_cross_pedestrian_crossing_without_lights();
    void cross_pedestrian_crossing();

    void ride_to_turn();
    void take_a_turn();
    void take_left_turn();
    void take_right_turn();

    void turn_around();

    void ride_to_crossing_with_lights();
    void ride_to_crossing_without_lights();
    void get_closer_to_crossing();
    void wait_to_cross_crossing_with_lights();
    void wait_to_cross_crossing_without_lights();
    void ride_through_crossing();

    //Helper methods
    /**
     * @brief handle_incoming_collisions - checks for collision possibility in immediate vicinity and further distance based on spare space
     * argument's value and acts accordingly
     * @return returns true if incoming collision was handled; otherwise returns false (no collision to handle)
     */
    bool handle_incoming_collisions(int spare_space);
    double calculate_possible_acceleration();
    bool check_for_p_crossing_lights();
    bool check_for_crossing_lights();
    bool check_for_pedestrians_trying_to_cross();
    void consider_traffic_signs();
    void consider_traffic_sign(CTrafficSign *traffic_sign);
    QPointF get_current_cell_pos();
    double round_current_speed_down_to_limit();

    void choose_direction_on_crossing_without_lights();
    void choose_direction_on_crossing_with_lights();
    double get_distance_to_stationary_map_element_in_straight_line(CStationaryMapElement *map_element);
    QPointF get_new_possible_pos_while_accelerating();
    QVector<int> get_allowed_crossing_directions();
    bool check_if_turning_left_on_crossing_with_lights_is_possible();

    bool check_if_driving_straight_through_crossing_without_lights_is_possible();
    bool check_if_driving_left_through_crossing_without_lights_is_possible();
    bool check_if_driving_right_through_crossing_without_lights_is_possible();
    bool check_if_turning_around_is_possible(CRoadway *roadway);
    void match_direction_to_the_destination_point();
    void correct_current_pos_in_regard_to_roadway();
    int find_crossing_side_to_destination(QVector<int> allowed_directions);
    double normal_speed_to_simulation_speed(int speed);
};

#endif // CCAR_H
