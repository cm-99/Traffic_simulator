#ifndef CROADUSER_H
#define CROADUSER_H

#include <QPointF>
#include <QGraphicsPixmapItem>

enum  ERoadUsers {car, pedestrian};
enum  EHorizontalMoveDirection {left, right};
enum  EVerticalMoveDirection {up, down};

/**
 * @brief The SRoadUsersBasicParameters struct can be used to pass a full set of CRoadUser's basic parameters (attributes).
 */
struct SRoadUsersBasicParameters{
    /**
     * @brief m_forward_visibility_distance is a distance at which the road user can see and identify objects in front of him
     */
    int m_forward_visibility_distance;
    /**
     * @brief m_lateral_visibility_range is used to determine which objects are seen by the road user in relation to the road width
     *
     * By default it should be set so that road user can discern elements on the whole width of the road
    **/
    int m_lateral_visibility_distance;
    int m_reaction_time;
    int m_max_acceleration_value;
    int m_max_deceleration_value;
    int m_max_speed;
    int m_chance_of_breaking_law;

    explicit SRoadUsersBasicParameters(){};
    SRoadUsersBasicParameters(int forward_visibility_distance, int lateral_visibility_distance, int reaction_time,
                              int max_acceleration_value, int max_deceleration_value, int max_speed, int chance_of_breaking_law = 0)
    {
        m_forward_visibility_distance = forward_visibility_distance;
        m_lateral_visibility_distance = lateral_visibility_distance;
        m_reaction_time = reaction_time;
        m_max_acceleration_value = max_acceleration_value;
        m_max_deceleration_value = max_deceleration_value;
        m_max_speed = max_speed;
        m_chance_of_breaking_law = chance_of_breaking_law;
    }

    SRoadUsersBasicParameters(QVector<int> attributes_values){
        m_forward_visibility_distance = attributes_values[0];
        m_lateral_visibility_distance = attributes_values[1];
        m_reaction_time = attributes_values[2];
        m_max_acceleration_value = attributes_values[3];
        m_max_deceleration_value = attributes_values[4];
        m_max_speed = attributes_values[5];
        m_chance_of_breaking_law = attributes_values[6];
    }

    static int get_attributes_number(){
        return 7;
    }

    static QStringList get_attributes_names_list(){
        return QStringList{"Forward visibility distance [m]", "Lateral visibility distance [m]", "Reaction time [ms]",
                           "Max acceleration value [m/s^2]", "Max deceleration value [m/s^2]", "Max speed [m/s]",
                           "Chance of breaking law [%]"};
    }

    static std::tuple<int, int, int> get_attribute_min_max_and_mean_values(int index){
        switch (index){
        case 0:
            return std::tuple<int, int, int>(0, 100, 50);
            break;
        case 1:
            return std::tuple<int, int, int>(0, 50, 10);
            break;
        case 2:
            return std::tuple<int, int, int>(700, 2000, 1000);
            break;
        case 3:
            return std::tuple<int, int, int>(1, 8, 4);
            break;
        case 4:
            return std::tuple<int, int, int>(1, 6, 4);
            break;
        case 5:
            return std::tuple<int, int, int>(150, 400, 200);
            break;
        case 6:
            return std::tuple<int, int, int>(0, 100, 0);
            break;
        default:
            throw std::invalid_argument("Invalid value of argument index (>= get_attributes_number()");
            break;
        }
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
    EHorizontalMoveDirection m_horizontal_move_direction;
    EVerticalMoveDirection m_vertical_move_direction;
    double m_current_speed{0};
    ERoadUsers m_road_user_type;
};

#endif // CROADUSER_H
