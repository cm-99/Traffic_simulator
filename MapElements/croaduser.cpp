#include "MapElements/croaduser.h"

void CRoadUser::set_basic_parameters(SRoadUsersBasicParameters parameters)
{
    m_road_users_parameters = parameters;

    m_road_users_parameters_adjusted_to_step = parameters;

    //Assuming - simulation step = 100 ms
    //max_acceleration - From m/s^2 to pixels/step^2 - 10*pixel /(10 step * 10 step) = 10*pixel/100*step = (1/10)*(pixel/step)
    //max_deceleration - same as acceleration
    m_road_users_parameters_adjusted_to_step.m_max_acceleration_value = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value/10;
    m_road_users_parameters_adjusted_to_step.m_max_deceleration_value = m_road_users_parameters_adjusted_to_step.m_max_deceleration_value/10;
    //max_speed - From km/h to pixels/step - 10 000*pixel/(3600 * 10 step) = (10 000/36 000)*(pixel/step)
    m_road_users_parameters_adjusted_to_step.m_max_speed = (m_road_users_parameters_adjusted_to_step.m_max_speed * 10)/36;
    //reaction_time - discretized to steps number
    m_road_users_parameters_adjusted_to_step.m_reaction_time = m_road_users_parameters_adjusted_to_step.m_reaction_time/100;
}

void CRoadUser::update_speed_vector()
{
    int rotation_in_degrees = this->rotation();
    double radians = (rotation_in_degrees * M_PI)/180;
    m_speed_vector = {cos(radians) * m_current_speed, sin(radians) * m_current_speed};
}

void CRoadUser::set_rotation_by_center(double degrees)
{
    QPointF center = this->boundingRect().center();
    this->setTransformOriginPoint(center);
    this->setRotation(degrees);
}

QPoint CRoadUser::grid_point_to_map_point(QPoint grid_point)
{
    return QPoint(m_default_cell_size.width() * grid_point.y(), m_default_cell_size.height() * grid_point.x());
}
