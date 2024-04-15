#ifndef CROADUSER_H
#define CROADUSER_H

#include <QPointF>
#include <QGraphicsPixmapItem>

enum class ERoadUsers {car, pedestrian};
enum class EMoveDirection {left, right, up, down};

/**
 * @brief The SRoadUsersBasicParameters struct can be used to pass a full set of CRoadUser's basic parameters (attributes).
 */
struct SRoadUsersBasicParameters{
    /**
     * @brief m_forward_visibility_distance is a distance at which the road user can see and identify objects in front of him
     */
    double m_forward_visibility_distance;
    /**
     * @brief m_lateral_visibility_range is used to determine which objects are seen by the road user in relation to the road witdth
     *
     * By default it should be set so that road user can discern elements on the whole width of the road
    **/
    double m_lateral_visibility_range;
    double m_max_acceleration_value;
    double m_max_deceleration_value;
    double m_max_speed;
    int m_chance_of_breaking_law;

    explicit SRoadUsersBasicParameters(){};
    SRoadUsersBasicParameters(double forward_visibility_distance, double lateral_visibility_range, double max_acceleration_value,
    double max_deceleration_value, double max_speed, int chance_of_breaking_law = 0)
    {
        m_forward_visibility_distance = forward_visibility_distance;
        m_lateral_visibility_range = lateral_visibility_range;
        m_max_acceleration_value = max_acceleration_value;
        m_max_deceleration_value = max_deceleration_value;
        m_max_speed = max_speed;
        m_chance_of_breaking_law = chance_of_breaking_law;
    }
};

/**
 * @brief The CRoadUser class is a base class for road users representation.
 * Enables the creation of graphical representation of road user and stores the values of related simulation parameters.
 *
 * POSSIBLE future functionality - decrease/widen visibility range based on weather/road user's conditions to widen simulation options e.g.
 * - for lateral visibility it might mean not seeing a pedestrian nearing the crossing; not seeing a sign on the side of the road
 * - for forward visibility it might mean not discerning sign on the side of the road fast enough
 */
class CRoadUser: public QGraphicsPixmapItem
{
public:
    CRoadUser(ERoadUsers road_user_type) :
        m_road_user_type(road_user_type) {setZValue(1);}
    /**
     * @brief move is a main method for simulation purposes - it should cause the movement of a given object.
     */
    virtual void move() = 0;

    const SRoadUsersBasicParameters& get_basic_parameters() const {return m_road_users_parameters;}
    ERoadUsers get_road_user_type() const {return m_road_user_type;}
    // TODO: IT SHOULD BE PROTECTED FROM UNAUTHORIZED EDITION. THIS SHOULD STAY ONLY FOR TESTS.
    // SRoadUsersBasicParameters& get_editable_basic_parameters() {return m_road_users_parameters;}

protected:
    SRoadUsersBasicParameters m_road_users_parameters;
    /**
     * @brief m_designated_destination is to be used when implementing movement to designated point chosen by the user.
     */
    QPointF m_designated_destination;
    bool m_has_designated_destination{false};
    bool m_is_involved_in_accident{false};
    bool m_is_selected{false};
    EMoveDirection m_move_direction;
    double m_current_speed{0};
    ERoadUsers m_road_user_type;
};

#endif // CROADUSER_H
