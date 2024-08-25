#ifndef CROADUSER_H
#define CROADUSER_H

#include "MapElements/StationaryMapElements/croadelement.h"
#include <QPointF>
#include <QGraphicsPixmapItem>

enum  ERoadUsers {car, pedestrian};
enum  EHorizontalMoveDirection {left, right};
enum  EVerticalMoveDirection {up, down};

class CReadOnlyMap;

/**
 * @brief The SRoadUsersBasicParameters struct can be used to pass a full set of CRoadUser's basic parameters (attributes).
 */
struct SRoadUsersBasicParameters{
    /**
     * @brief m_forward_visibility_distance is a distance at which the road user can see and identify objects in front of him
     */
    double m_forward_visibility_distance;
    /**
     * @brief m_lateral_visibility_range is used to determine which objects are seen by the road user in relation to the road width
     *
     * By default it should be set so that road user can discern elements on the whole width of the road
    **/
    double m_lateral_visibility_distance;
    double m_reaction_time;
    double m_max_acceleration_value;
    double m_max_deceleration_value;
    double m_max_speed;
    double m_chance_of_breaking_law;
    bool m_is_empty{true};

    explicit SRoadUsersBasicParameters(){};
    SRoadUsersBasicParameters(double forward_visibility_distance, double lateral_visibility_distance, double reaction_time,
                              double max_acceleration_value, double max_deceleration_value, double max_speed, double chance_of_breaking_law = 0)
    {
        m_forward_visibility_distance = forward_visibility_distance;
        m_lateral_visibility_distance = lateral_visibility_distance;
        m_reaction_time = reaction_time;
        m_max_acceleration_value = max_acceleration_value;
        m_max_deceleration_value = max_deceleration_value;
        m_max_speed = max_speed;
        m_chance_of_breaking_law = chance_of_breaking_law;
        m_is_empty = false;
    }

    SRoadUsersBasicParameters(QVector<double> attributes_values){
        m_forward_visibility_distance = attributes_values[0];
        m_lateral_visibility_distance = attributes_values[1];
        m_reaction_time = attributes_values[2];
        m_max_acceleration_value = attributes_values[3];
        m_max_deceleration_value = attributes_values[4];
        m_max_speed = attributes_values[5];
        m_chance_of_breaking_law = attributes_values[6];
        m_is_empty = false;
    }

    bool is_empty() const {return m_is_empty;}

    static int get_attributes_number(){
        return 7;
    }

    double get_attribute_by_index(int index){
        switch (index){
        case 0:
            return m_forward_visibility_distance;
            break;
        case 1:
            return m_lateral_visibility_distance;
            break;
        case 2:
            return m_reaction_time;
            break;
        case 3:
            return m_max_acceleration_value;
            break;
        case 4:
            return m_max_deceleration_value;
            break;
        case 5:
            return m_max_speed;
            break;
        case 6:
            return m_chance_of_breaking_law;
            break;
        default:
            throw std::out_of_range("Value of argument index out of range (>= get_attributes_number())");
        }
    }

    static QStringList get_attributes_names_list(){
        return QStringList{"Forward visibility distance [m]", "Lateral visibility distance [m]", "Reaction time [ms]",
                           "Max acceleration value [m/s^2]", "Max deceleration value [m/s^2]", "Max speed [km/h]",
                           "Chance of breaking law [%]"};
    }

    static std::tuple<double, double, double> get_road_users_attributes_min_max_and_mean_values(int index, ERoadUsers road_user_type){
        if(road_user_type == ERoadUsers::car){
            switch (index){
            case 0:
                return std::tuple<double, double, double>(0, 5000, 3000);
                break;
            case 1:
                return std::tuple<double, double, double>(0, 5000, 3000);
                break;
            case 2:
                return std::tuple<double, double, double>(700, 2000, 1000);
                break;
            case 3:
                return std::tuple<double, double, double>(1, 8, 4);
                break;
            case 4:
                return std::tuple<double, double, double>(1, 6, 4);
                break;
            case 5:
                return std::tuple<double, double, double>(150, 400, 200);
                break;
            case 6:
                return std::tuple<double, double, double>(0, 100, 0);
                break;
            default:
                throw std::out_of_range("Value of argument index out of range (>= get_attributes_number())");
                break;
            }
        }
        else{
            switch (index){
            case 0:
                return std::tuple<double, double, double>(0, 5000, 3000);
                break;
            case 1:
                return std::tuple<double, double, double>(0, 5000, 3000);
                break;
            case 2:
                return std::tuple<double, double, double>(700, 2000, 1000);
                break;
            case 3:
                return std::tuple<double, double, double>(0.34, 1.44, 0.68);
                break;
            case 4:
                return std::tuple<double, double, double>(2, 5, 3);
                break;
            case 5:
                return std::tuple<double, double, double>(3, 27, 6);
                break;
            case 6:
                return std::tuple<double, double, double>(0, 100, 0);
                break;
            default:
                throw std::out_of_range("Value of argument index out of range (>= get_attributes_number())");
                break;
            }
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
    CRoadUser(ERoadUsers road_user_type, QString description, EMovementPlane movement_plane,
              EHorizontalMoveDirection horizontal_move_direction, EVerticalMoveDirection vertical_move_direction) :
        m_road_user_type(road_user_type),
        m_description(description),
        m_horizontal_move_direction(horizontal_move_direction),
        m_vertical_move_direction(vertical_move_direction),
        m_current_movement_plane(movement_plane)
        {setZValue(4);}
    /**
     * @brief move is a main method for simulation purposes - it should cause the movement of a given object according to traffic laws.
     */
    virtual void move(CReadOnlyMap *map) = 0;

    void set_basic_parameters(SRoadUsersBasicParameters parameters);
    inline void set_current_movement_plane(EMovementPlane mp) {m_current_movement_plane = mp;}
    inline void set_horizontal_move_direction(EHorizontalMoveDirection hmd) {m_horizontal_move_direction = hmd;}
    inline void set_vertical_move_direction(EVerticalMoveDirection vmd) {m_vertical_move_direction = vmd;}
    virtual void set_destination(QPointF destination, std::vector<QPoint> path) = 0;

    inline const SRoadUsersBasicParameters& get_basic_parameters() const {return m_road_users_parameters;}
    inline ERoadUsers get_road_user_type() const {return m_road_user_type;}
    inline QString get_description() const {return m_description;}
    inline EMovementPlane get_current_movement_plane() const {return m_current_movement_plane;}
    inline EHorizontalMoveDirection get_horizontal_move_direction() const {return m_horizontal_move_direction;}
    inline EVerticalMoveDirection get_vertical_move_direction() const {return m_vertical_move_direction;}
    inline bool has_designated_destination() const {return m_has_designated_destination;}
    inline QPointF get_destination() const {return m_destination;}
    inline bool is_rotable() const {return m_is_rotable;}

protected:
    SRoadUsersBasicParameters m_road_users_parameters;
    SRoadUsersBasicParameters m_road_users_parameters_adjusted_to_step;
    CReadOnlyMap *m_map;
    QSize m_default_cell_size;
    /**
     * @brief m_designated_destination is to be used when implementing movement to designated point chosen by the user.
     */
    QPointF m_destination;
    std::vector<QPoint> m_path_to_destination;
    bool m_has_designated_destination{false};

    bool m_is_involved_in_accident{false};
    bool m_is_rotable{true};

    ERoadUsers m_road_user_type;
    QString m_description;

    EHorizontalMoveDirection m_horizontal_move_direction;
    EVerticalMoveDirection m_vertical_move_direction;
    EMovementPlane m_current_movement_plane;

    double m_current_speed{0};
    std::pair<double, double> m_speed_vector{0, 0};
    void update_speed_vector();
    void set_rotation_by_center(double degrees);
    QPoint grid_point_to_map_point(QPoint grid_point);
};

#endif // CROADUSER_H
