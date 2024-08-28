#include "MapElements/croaduser.h"
#include "Logic/MapModels/creadonlymap.h"

void CRoadUser::set_basic_parameters(SRoadUsersBasicParameters parameters)
{
    m_road_users_parameters = parameters;

    m_road_users_parameters_adjusted_to_step = parameters;

    //!!! simulation step = 100 ms, m = 10 pixels
    m_road_users_parameters_adjusted_to_step.m_forward_visibility_distance = m_road_users_parameters_adjusted_to_step.m_forward_visibility_distance*10;
    m_road_users_parameters_adjusted_to_step.m_lateral_visibility_distance = m_road_users_parameters_adjusted_to_step.m_lateral_visibility_distance*10;
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

void CRoadUser::change_rotation_by_center(double degrees)
{
    QPointF center = this->boundingRect().center();
    this->setTransformOriginPoint(center);
    this->setRotation(this->rotation() + degrees);
}

QPoint CRoadUser::grid_point_to_map_point(QPoint grid_point)
{
    return QPoint(m_default_cell_size.width() * grid_point.y() + m_default_cell_size.width()/2,
                  m_default_cell_size.height() * grid_point.x() + m_default_cell_size.height()/2);
}

QPoint CRoadUser::map_point_to_grid_point(QPoint map_point)
{
    auto cell_size = m_map->get_default_cell_size();
    int grid_y_pos = std::div(map_point.x(), cell_size.width()).quot;
    int grid_x_pos = std::div(map_point.y(), cell_size.height()).quot;

    return QPoint(grid_x_pos, grid_y_pos);
}

bool CRoadUser::is_out_of_bounds(CReadOnlyMap *map_model)
{
    if(this->pos().x() < 0 || this->pos().x() > map_model->get_scene_rect().width() || this->pos().y() < 0 ||
        this->pos().y() > map_model->get_scene_rect().height()){
        m_is_out_of_bounds = true;
        return true;
    }
    else{
        return false;
    }
}
