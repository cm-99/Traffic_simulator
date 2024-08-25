#ifndef CPEDESTRIAN_H
#define CPEDESTRIAN_H

#include "MapElements/croaduser.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"

enum EPedestrianMovementStates{walking, waiting_to_cross_pedestrian_crossing_with_lights, waiting_to_cross_pedestrian_crossing_without_lights,
                                 crossing_pedestrian_crossing, moving_to_destination};
enum EPedestrianMovementStatesToDestination{walking_to_destination};

/**
 * @brief The CPedestrian class is a graphical and behavioral representation of a pedestrian.
 */
class CPedestrian : public CRoadUser
{
public:
    static CRoadUser *create_pedestrian();
    virtual void move(CReadOnlyMap *map) override;

private:
    /**
     * @brief CPedestrian
     * @param pedestrian_states_pixmaps - vector should contain 3 pixmaps in order - stationary pedestrian, pedestrian taking a step
     * from one leg and pedestrian taking a step from another leg.
     */
    CPedestrian(QVector<QPixmap> pedestrian_states_pixmaps, QString description, EMovementPlane movement_plane,
         EHorizontalMoveDirection horizontal_move_direction = EHorizontalMoveDirection::left,
         EVerticalMoveDirection vertical_move_direction = EVerticalMoveDirection::up);

    QVector<QPixmap> m_pedestrian_states_pixmaps;
    int m_current_step_state{0};

    EPedestrianMovementStates m_current_movement_state{EPedestrianMovementStates::walking};
    EPedestrianMovementStatesToDestination m_current_movement_state_to_destination{EPedestrianMovementStatesToDestination::walking_to_destination};

    bool is_omitting_p_crossing{false};
    int p_crossing_omission_steps{0};

    CTrafficLight *m_crossing_light{nullptr};
    CRoadElement *m_p_crossing{nullptr};

    QMap<EPedestrianMovementStatesToDestination, void (CPedestrian::*)()> m_movement_states_to_destination_mapped_to_func;
    QMap<EPedestrianMovementStates, void (CPedestrian::*)()> m_movement_states_mapped_to_func;

    void set_destination(QPointF destination, std::vector<QPoint> path) override;

    //Methods for movement to selected destination
    void move_to_destination();
    void take_next_step_to_destination();

    //Methods for random movement
    void take_next_step();
    void cross_pedestrian_crossing();
    void wait_to_cross_pedestrian_crossing_with_lights();
    void wait_to_cross_pedestrian_crossing_without_lights();

    //Helper methods
    bool check_if_crossing_pedestrian_crossing_with_lights_is_possible(CRoadElement *p_crossing, CTrafficLight *traffic_light);
    bool check_if_crossing_pedestrian_crossing_without_lights_is_possible(CRoadElement *p_crossing);
    double get_possible_acceleration();
};

#endif // CPEDESTRIAN_H
