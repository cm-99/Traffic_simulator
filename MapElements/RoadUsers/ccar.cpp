#include "ccar.h"
#include "Logic/MapModels/creadonlymap.h"
#include "MapElements/RoadUsers/cpedestrian.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/croadway.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"
#include "MapElements/StationaryMapElements/cfiller.h"
#include <random>

CCar::CCar(QPixmap car_pixmap, QString description, EMovementPlane movement_plane, EHorizontalMoveDirection horizontal_move_direction, EVerticalMoveDirection vertical_move_direction) :
    CRoadUser(ERoadUsers::car, description, movement_plane, horizontal_move_direction, vertical_move_direction)
{
    setPixmap(car_pixmap);
    m_default_cell_size = CReadOnlyMap::get_default_cell_size();
    m_movement_states_mapped_to_func[ECarMovementStates::riding] = &CCar::ride;

    m_movement_states_mapped_to_func[ECarMovementStates::riding_to_pedestrian_crossing_with_lights] = &CCar::ride_to_pedestrian_crossing_with_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::riding_to_pedestrian_crossing_without_lights] = &CCar::ride_to_pedestrian_crossing_without_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::waiting_to_cross_pedestrian_crossing_w_lights] = &CCar::wait_to_cross_pedestrian_crossing_with_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::waiting_to_cross_pedestrian_crossing_wi_lights] = &CCar::wait_to_cross_pedestrian_crossing_without_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::crossing_p_crossing] = &CCar::cross_pedestrian_crossing;

    m_movement_states_mapped_to_func[ECarMovementStates::riding_to_crossing_with_lights] = &CCar::ride_to_crossing_with_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::riding_to_crossing_without_lights] = &CCar::ride_to_crossing_without_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::getting_closer_to_the_crossing] = &CCar::get_closer_to_crossing;
    m_movement_states_mapped_to_func[ECarMovementStates::waiting_to_cross_crossing_with_lights] = &CCar::wait_to_cross_crossing_with_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::waiting_to_cross_crossing_without_lights] = &CCar::wait_to_cross_crossing_without_lights;
    m_movement_states_mapped_to_func[ECarMovementStates::riding_through_crossing] = &CCar::ride_through_crossing;

    m_movement_states_mapped_to_func[ECarMovementStates::riding_to_turn] = &CCar::ride_to_turn;
    m_movement_states_mapped_to_func[ECarMovementStates::taking_turn] = &CCar::take_a_turn;
    m_movement_states_mapped_to_func[ECarMovementStates::taking_left_turn] = &CCar::take_left_turn;
    m_movement_states_mapped_to_func[ECarMovementStates::taking_right_turn] = &CCar::take_right_turn;

    m_movement_states_mapped_to_func[ECarMovementStates::turning_around] = &CCar::turn_around;
}

void CCar::match_direction_to_the_destination_point(){

    QPoint center_point = QPoint(this->pos().x() + this->boundingRect().width()/2,
                                 this->pos().y() + this->boundingRect().height()/2);
    QPoint current_grid_point = map_point_to_grid_point(center_point);

    if(current_grid_point != m_path_to_destination[0]){
        destination_reached();
        return;
    }

    bool direction_found = false;

    if(m_path_to_destination.size() == 1){
        destination_reached();
        return;
    }
    else{
        //Which way to go
        //grid_map -> y and x are swapped relative to map

        if(m_current_movement_plane == EMovementPlane::horizontal && (current_grid_point.x() == m_path_to_destination[1].x())){
            //Still the same movement plane, check fo direction
            direction_found = true;

            if(m_horizontal_move_direction == EHorizontalMoveDirection::left){
                //Next point already behind, turn around
                if(current_grid_point.y() < m_path_to_destination[1].y()){
                    m_current_movement_state = ECarMovementStates::turning_around;
                }
                else{
                    return; //Good for now
                }
            }
            else{
                if(current_grid_point.y() > m_path_to_destination[1].y()){
                    m_current_movement_state = ECarMovementStates::turning_around;
                }
                else{
                    return; //Good for now
                }
            }
        }
        else if(m_current_movement_plane == EMovementPlane::vertical && (current_grid_point.y() == m_path_to_destination[1].y())){
            direction_found = true;

            if(m_vertical_move_direction == EVerticalMoveDirection::up){

                if(current_grid_point.x() < m_path_to_destination[1].x()){
                    m_current_movement_state = ECarMovementStates::turning_around;
                }
                else{
                    return; //Good for now
                }
            }
            else{
                if(current_grid_point.y() > m_path_to_destination[1].x()){
                    m_current_movement_state = ECarMovementStates::turning_around;
                }
                else{
                    return; //Good for now
                }
            }
        }
    }

    if(!direction_found){
        destination_reached();
        return;
    }
}

void CCar::correct_current_pos_in_regard_to_roadway()
{
    QPointF car_center_pos = this->pos();
    car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                             car_center_pos.y() + this->boundingRect().height()/2);

    int cars_roadside_offset = 5;
    auto colliding_items = m_map->collidingItems(this);
    CRoadway *roadway = nullptr;
    for(auto colliding_item : colliding_items){
        auto maybe_roadway = dynamic_cast<CRoadway*>(colliding_item);
        if(maybe_roadway && maybe_roadway->get_movement_plane() == this->m_current_movement_plane){
            roadway = maybe_roadway;
            break;
        }
    }

    if(roadway == nullptr){
        //Far off the roadway, try with offset
        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                this->setPos(this->pos().x(), this->pos().y() - 20);
            }
            else{
                this->setPos(this->pos().x(), this->pos().y() + 20);
            }
        }
        else{

            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                this->setPos(this->pos().x() - 20, this->pos().y());
            }
            else{
                this->setPos(this->pos().x() + 20, this->pos().y());
            }
        }

        auto colliding_items = m_map->collidingItems(this);
        for(auto colliding_item : colliding_items){
            auto maybe_roadway = dynamic_cast<CRoadway*>(colliding_item);
            if(maybe_roadway && maybe_roadway->get_movement_plane() == this->m_current_movement_plane){
                roadway = maybe_roadway;
                break;
            }
        }
        if(roadway == nullptr){
            //Something went terribly wrong
            return;
        }
    }

    auto rodaway_test = roadway->boundingRect();
    auto roadway_rect = rodaway_test.toRect();
    auto roadway_width = roadway_rect.width();
    auto roadway_height = roadway_rect.height();

    int x_roadway_offset = 0;
    int y_roadway_offset = 0;

    if(abs((int)roadway->rotation()) % 180 == 90 && roadway_height != roadway_width){
        x_roadway_offset = abs(roadway_width - roadway_height)/2;
        y_roadway_offset = abs(roadway_width - roadway_height)/2;
        roadway_width = roadway_height;
        roadway_height = roadway_rect.width();
    }

    QPoint real_roadway_pos = QPoint(roadway->pos().x() + x_roadway_offset, roadway->pos().y() - y_roadway_offset);

    auto car_bounding_rect = this->boundingRect();
    auto car_rect = car_bounding_rect.toRect();
    auto car_width = car_rect.width();
    auto car_height = car_rect.height();

    int x_car_offset = 0;
    int y_car_offset = 0;

    if(abs((int)this->rotation()) % 180 == 90 && car_width != car_height){
        x_car_offset = abs(car_width - car_height)/2;
        y_car_offset = abs(car_width - car_height)/2;
    }

    if((int)(this->rotation()) % 180 != 0){
        car_width = this->boundingRect().height();
        car_height = this->boundingRect().width();
    }

    //Place car on the proper lane
    if(this->get_current_movement_plane() == EMovementPlane::vertical){
        int y_car_pos = car_center_pos.y();
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            int x_car_pos = real_roadway_pos.x() - x_car_offset + roadway_width - car_width - cars_roadside_offset;
            this->setPos(x_car_pos, y_car_pos);
        }
        else{
            int x_car_pos = real_roadway_pos.x() - x_car_offset + cars_roadside_offset;
            this->setPos(x_car_pos, y_car_pos);
        }
    }
    else{
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            int y_car_pos = real_roadway_pos.y() + roadway_height - car_height - cars_roadside_offset - y_car_offset;
            this->setPos(this->pos().x(), y_car_pos);
        }
        else{
            int y_car_pos = real_roadway_pos.y() + cars_roadside_offset - y_car_offset;
            this->setPos(this->pos().x(), y_car_pos);
        }
    }
}

void CCar::set_destination(QPointF destination, std::vector<QPoint> path)
{
    m_has_designated_destination = true;
    m_destination = destination;
    m_path_to_destination = path;

    if(m_map == nullptr){
        m_was_waiting_for_simulation_to_start = true;
    }
    else{
        match_direction_to_the_destination_point();
    }
}

void CCar::destination_reached()
{
    m_current_path_point = 0;
    m_has_designated_destination = false;
}

void CCar::ride()
{
    if(m_was_waiting_for_simulation_to_start){
        match_direction_to_the_destination_point();
        m_was_waiting_for_simulation_to_start = false;
        return;
    }

    if(m_was_leaving_a_crossing){
        m_was_leaving_a_crossing = false;
        m_there_is_road_priority_sign_to_check = false;
        m_temporary_speed_limit = m_global_speed_limit;
        m_allowed_horizontal_movements = {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};
        m_allowed_vertical_movements = {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};
    }

    m_lights_present_but_inactive = false;

    bool collision_was_incoming = handle_incoming_collisions(15);
    consider_traffic_signs();

    QPointF new_possible_pos;

    if(collision_was_incoming){
        new_possible_pos = this->pos();
    }
    else{
        double acceleration_possible = calculate_possible_acceleration();
        m_current_speed = m_current_speed + acceleration_possible;

        int max_limit = m_global_speed_limit;
        if(m_temporary_speed_limit != -1){
            max_limit = m_temporary_speed_limit;
        }

        if(m_current_speed > max_limit){
            m_current_speed = max_limit;
        }

        update_speed_vector();
        new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }

    int line_of_sight_length = 400;
    if(line_of_sight_length > m_road_users_parameters_adjusted_to_step.m_forward_visibility_distance){
        line_of_sight_length = m_road_users_parameters_adjusted_to_step.m_forward_visibility_distance;
    }

    int cells_to_check = line_of_sight_length/(m_default_cell_size.width()/2);
    //Depending on movement plane, either increment next position in x or y axis
    int j = 0;
    int k = 0;

    if(this->get_current_movement_plane() == EMovementPlane::horizontal){

        //Check the pos in front of the car
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            j = m_default_cell_size.width()/2;
            new_possible_pos = QPointF(new_possible_pos.x() + this->boundingRect().width(),
                                       new_possible_pos.y() + this->boundingRect().height()/2);
        }
        else{
            j = -m_default_cell_size.width()/2;
            new_possible_pos = QPointF(new_possible_pos.x(),
                                       new_possible_pos.y() + this->boundingRect().height()/2);
        }
    }
    else{ //Vertical

        //Check the pos in front of the car
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::down){
            k = m_default_cell_size.width();
            new_possible_pos = QPointF(new_possible_pos.x() + this->boundingRect().width()/2,
                                       new_possible_pos.y() + this->boundingRect().width() - this->boundingRect().height()/2);
        }
        else{
            k = -m_default_cell_size.width();
            new_possible_pos = QPointF(new_possible_pos.x()  + this->boundingRect().width()/2,
                                       new_possible_pos.y() - this->boundingRect().width()/2 + this->boundingRect().height()/2);
        }
    }

    bool only_filler_spotted = false;

    for(int i = 0; i < cells_to_check; i++){

        new_possible_pos = QPointF(new_possible_pos.x() + j, new_possible_pos.y() + k);

        auto items_at_line_of_sight = m_map->items(new_possible_pos);
        for(auto item : items_at_line_of_sight){ //Road users should be handled by now

            auto stationary_map_element = dynamic_cast<CStationaryMapElement*>(item);

            if(!stationary_map_element){ //Weird but ok, continue
                continue; // CONTINUE
            }

            if(i == 0 && stationary_map_element->get_map_element_type() == EStationaryMapElementType::filler){//Dead end ahead
                only_filler_spotted = true;
            }

            auto road_element = dynamic_cast<CRoadElement*>(item);
            if(!road_element){
                continue; // CONTINUE
            }

            if(i == 0 && road_element->get_road_element_type() == ERoadElementType::pavement){ //Another dead end ahead, turn around
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::turning_around;
                return; // EARLY RETURN
            }

            // Handle whatever will be first in your way which is not a roadway
            if(road_element->get_road_element_type() == ERoadElementType::pedestrian_crossing){ //Handling pedestrian crossing in the way

                //Lights or no lights
                m_p_crossing = static_cast<CPedestrianCrossing*>(road_element);
                bool is_crossing_with_lights = check_for_p_crossing_lights();

                if(is_crossing_with_lights){
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::riding_to_pedestrian_crossing_with_lights;
                }
                else{
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::riding_to_pedestrian_crossing_without_lights;
                }

                //Check if there is a crossing right after pedestrian crossing
                new_possible_pos = QPointF(new_possible_pos.x() + j, new_possible_pos.y() + k);
                auto items_after_p_crossing = m_map->items(new_possible_pos);

                for(auto item : items_after_p_crossing){
                    auto crossing = dynamic_cast<CCrossing*>(item);

                    if(crossing){
                        m_crossing_after_p_crossing = true;
                        m_crossing = crossing;
                    }
                }

                return; // EARLY RETURN
            }

            if(road_element->get_road_element_type() == ERoadElementType::roadway_element){
                auto roadway_element = static_cast<CRoadwayElement*>(road_element);

                if(roadway_element->get_roadway_element_type() == ERoadwayElementType::roadway){ //Roadway is good, keep on checking
                    continue;
                }

                if(roadway_element->get_roadway_element_type() == ERoadwayElementType::turn){ //Handling the turn
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::riding_to_turn;
                    m_turn = static_cast<CTurn*>(roadway_element);
                    return; // EARLY RETURN
                }

                if(roadway_element->get_roadway_element_type() == ERoadwayElementType::crossing){ //Handling the crossing

                    m_crossing= static_cast<CCrossing*>(roadway_element);
                    bool is_crossing_with_lights = check_for_crossing_lights();

                    if(is_crossing_with_lights){
                        m_previous_movement_state = m_current_movement_state;
                        m_current_movement_state = ECarMovementStates::riding_to_crossing_with_lights;
                    }
                    else{ // no lights
                        m_previous_movement_state = m_current_movement_state;
                        m_current_movement_state = ECarMovementStates::riding_to_crossing_without_lights;
                    }

                    return; // EARLY RETURN
                }
            }

            if(i == 0 && only_filler_spotted){
                m_current_speed = 0;
                update_speed_vector();
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::turning_around;
            }
        }
    }
}

double CCar::get_distance_to_stationary_map_element_in_straight_line(CStationaryMapElement *map_element)
{
    double distance_to_element = 0;

    if(this->m_current_movement_plane == EMovementPlane::vertical){ //Car moving vertically, calculate by y axis
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){

            double map_element_y_bound = map_element->pos().y() + map_element->boundingRect().height();
            double car_front_bumper_pos = this->pos().y() - this->boundingRect().width()/2 + this->boundingRect().height()/2;
            distance_to_element = car_front_bumper_pos - map_element_y_bound;
        }
        else{
            double map_element_y_bound = map_element->pos().y();
            double car_front_bumper_pos = this->pos().y() + this->boundingRect().width()/2 + this->boundingRect().height()/2;
            distance_to_element = map_element_y_bound - car_front_bumper_pos;
        }
    }
    else{ //Car moving horizontally, calculate by x axis
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            double map_element_x_bound = map_element->pos().x();
            double car_front_bumper_pos = this->pos().x() + this->boundingRect().width();
            distance_to_element = map_element_x_bound - car_front_bumper_pos;
        }
        else{
            double map_element_x_bound = map_element->pos().x() + map_element->boundingRect().width();
            double car_front_bumper_pos = this->pos().x();
            distance_to_element = car_front_bumper_pos - map_element_x_bound;
        }
    }

    return distance_to_element;
}

QPointF CCar::get_new_possible_pos_while_accelerating()
{
    double acceleration_possible = calculate_possible_acceleration();
    m_current_speed = m_current_speed + acceleration_possible;
    round_current_speed_down_to_limit();
    update_speed_vector();

    return QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
}

void CCar::ride_to_pedestrian_crossing_with_lights()
{
    if(!(m_light->is_active())){ //Active->inactive
        m_lights_present_but_inactive = true;
        m_current_movement_state = ECarMovementStates::riding_to_crossing_without_lights;
        return;
    }

    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    //Distance to front bumper
    double distance_to_p_crossing = get_distance_to_stationary_map_element_in_straight_line(m_p_crossing);
    double distance_with_spare_space = distance_to_p_crossing - 5;

    if(m_light->get_current_lights_phase() != ETrafficLightsPhase::green){
        double deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

        if(deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value && !collision_handled){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 100);
            int sanity_value = dist(gen);

            if(sanity_value >= m_road_users_parameters.m_chance_of_breaking_law){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
                m_is_braking = true;

                if(distance_with_spare_space < 10){
                    m_current_speed = 0;
                    update_speed_vector();
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_w_lights;
                    m_is_waiting = true;
                    m_is_braking = false;
                    return;
                }
            }
            else{
                //Try to slam through, no speed limits for you
                double acceleration_possible = calculate_possible_acceleration();
                m_current_speed = m_current_speed + acceleration_possible;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);

                if(distance_with_spare_space < 10){
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::crossing_p_crossing;
                    m_is_waiting = false;
                    m_is_braking = false;
                    return;
                }
            }
        }
        else{
            //Check if you can accelerate some more
            QPointF new_possible_pos;
            double previous_speed = m_current_speed;
            QPointF previous_pos = this->pos();

            if(collision_handled){
                new_possible_pos = this->pos();
            }
            else{
                new_possible_pos = get_new_possible_pos_while_accelerating();
                this->setPos(new_possible_pos);
            }

            double distance_to_p_crossing = get_distance_to_stationary_map_element_in_straight_line(m_p_crossing);
            double distance_with_spare_space = distance_to_p_crossing - 5;
            double new_deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

            if(new_deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value){
                m_current_speed = previous_speed;
                this->setPos(previous_pos);

                if(!collision_handled){
                    m_current_speed = previous_speed - deceleration_needed;
                    update_speed_vector();
                    QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                    this->setPos(new_possible_pos);
                }
            }
            else{
                //If possible, brake
                if(!collision_handled){
                    m_current_speed = m_current_speed - deceleration_needed;
                    update_speed_vector();
                    QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                    this->setPos(new_possible_pos);
                    m_is_braking = true;
                }
            }

            //Stopping is possible, you don't have lights to cross, stop
            if(distance_to_p_crossing < 10){
                m_current_speed = 0;
                update_speed_vector();
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_w_lights;
                m_is_waiting = true;
                m_is_braking = false;
                return;
            }
        }
    }
    else{
        if(distance_to_p_crossing < 10){
            m_previous_movement_state = m_current_movement_state;
            m_current_movement_state = ECarMovementStates::crossing_p_crossing;
            m_is_waiting = false;
            m_is_braking = false;
            return;
        }

        if(collision_handled){
            return;
        }

        double acceleration_possible = calculate_possible_acceleration();
        m_current_speed = m_current_speed + acceleration_possible;
        round_current_speed_down_to_limit();

        update_speed_vector();
        QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }
}

void CCar::ride_to_pedestrian_crossing_without_lights()
{
    if(m_lights_present_but_inactive){
        if(m_light->is_active()){ // Inactive -> active
            m_lights_present_but_inactive = false;
            m_current_movement_state = ECarMovementStates::riding_to_crossing_with_lights;
            return;
        }
    }

    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    double distance_to_p_crossing = get_distance_to_stationary_map_element_in_straight_line(m_p_crossing);
    double distance_with_spare_space = distance_to_p_crossing - 5;
    bool pedestrians_to_let_spotted = check_for_pedestrians_trying_to_cross();

    if(pedestrians_to_let_spotted){

        double deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

        if(deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value && !m_crossing_after_p_crossing && !collision_handled){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 100);
            int sanity_value = dist(gen);

            if(sanity_value >= m_road_users_parameters.m_chance_of_breaking_law){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
                m_is_braking = true;

                if(distance_with_spare_space < 10){
                    m_current_speed = 0;
                    update_speed_vector();
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_wi_lights;
                    m_is_waiting = true;
                    m_is_braking = false;
                    return;
                }
            }
            else{
                //Try to slam through, no speed limits for you
                double acceleration_possible = calculate_possible_acceleration();
                m_current_speed = m_current_speed + acceleration_possible;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);

                if(distance_with_spare_space < 10){
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::crossing_p_crossing;
                    m_is_waiting = false;
                    m_is_braking = false;
                    return;
                }
            }
        }
        else{
            //If possible, or there will be a crossing (do not slam through it...) brake
            m_is_braking = true;

            if(!collision_handled){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
            }

            if(distance_with_spare_space < 10){
                m_current_speed = 0;
                update_speed_vector();
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_wi_lights;
                m_is_waiting = true;
                m_is_braking = false;
                return;
            }
        }
    }
    else{
        QPointF new_possible_pos;

        if(collision_handled){
            new_possible_pos = this->pos();
        }
        else{
            double acceleration_possible = calculate_possible_acceleration();
            acceleration_possible = acceleration_possible*0.7;
            m_current_speed = m_current_speed + acceleration_possible;
            round_current_speed_down_to_limit();
            update_speed_vector();

            new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
            this->setPos(new_possible_pos);
        }

        if(distance_with_spare_space < 10){
            m_previous_movement_state = m_current_movement_state;
            m_current_movement_state = ECarMovementStates::crossing_p_crossing;
            m_is_waiting = false;
            m_is_braking = false;
            return;
        }
    }
}

void CCar::wait_to_cross_pedestrian_crossing_with_lights()
{
    if(!(m_light->is_active())){ //Active->inactive
        m_lights_present_but_inactive = true;
        m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_wi_lights;
        return;
    }

    m_current_speed = 0;
    update_speed_vector();

    if(m_light->get_current_lights_phase() == ETrafficLightsPhase::green){
        m_is_waiting = false;
        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::crossing_p_crossing;
    }
}

void CCar::wait_to_cross_pedestrian_crossing_without_lights()
{
    if(m_lights_present_but_inactive){
        if(m_light->is_active()){ // Inactive -> active
            m_lights_present_but_inactive = false;
            m_current_movement_state = ECarMovementStates::waiting_to_cross_pedestrian_crossing_w_lights;
            return;
        }
    }

    m_current_speed = 0;
    update_speed_vector();

    bool crossing_possible = !(check_for_pedestrians_trying_to_cross());
    if(crossing_possible){
        m_is_waiting = false;
        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::crossing_p_crossing;
    }
}

void CCar::cross_pedestrian_crossing()
{
    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    if(m_crossing_after_p_crossing){
        if(m_previous_movement_state == ECarMovementStates::riding_to_pedestrian_crossing_with_lights
            || m_previous_movement_state == ECarMovementStates::waiting_to_cross_pedestrian_crossing_w_lights){

            m_previous_movement_state = ECarMovementStates::waiting_to_cross_crossing_with_lights;
            m_current_movement_state = ECarMovementStates::getting_closer_to_the_crossing;

        }
        else{
            m_previous_movement_state = ECarMovementStates::waiting_to_cross_crossing_without_lights;
            m_current_movement_state = ECarMovementStates::getting_closer_to_the_crossing;
        }
    }
    else{
        QPointF new_possible_pos;

        if(collision_handled){
            new_possible_pos = this->pos();
        }
        else{
            double acceleration_possible = calculate_possible_acceleration();
            m_current_speed = m_current_speed + acceleration_possible;
            m_current_speed = round_current_speed_down_to_limit();

            update_speed_vector();
            new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
            this->setPos(new_possible_pos);
        }

        bool got_out_of_p_crossing = true;
        auto items_at_pos = m_map->items(new_possible_pos);

        for(auto item : items_at_pos){ //Get out of pedestrian crossing, pavement is assured by validation
            auto p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

            if(p_crossing){
                got_out_of_p_crossing = false;
                break;
            }
        }

        if(got_out_of_p_crossing){
            m_previous_movement_state = m_current_movement_state;
            m_current_movement_state = ECarMovementStates::riding;
        }
    }
}

void CCar::ride_to_turn()
{
    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    double distance_to_turn = get_distance_to_stationary_map_element_in_straight_line(m_turn);
    double distance_with_spare_space = distance_to_turn - 5;

    double acceleration_possible = calculate_possible_acceleration();
    acceleration_possible = acceleration_possible*0.6;
    m_current_speed = m_current_speed + acceleration_possible;
    round_current_speed_down_to_limit();
    update_speed_vector();

    QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);

    if(!collision_handled){
        this->setPos(new_possible_pos);
    }

    if(distance_with_spare_space < 10){
        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::taking_turn;
        m_is_waiting = false;
        m_is_braking = false;
        return;
    }

}

void CCar::take_a_turn()
{
    EHorizontalMoveDirection turn_h_m_direction = m_turn->get_horizontal_direction_possible();
    EVerticalMoveDirection turn_v_m_direction = m_turn->get_vertical_direction_possible();
    m_turn_width = m_turn->boundingRect().width();

    if(m_current_movement_plane == EMovementPlane::horizontal){
        if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
            if(turn_h_m_direction == EHorizontalMoveDirection::right && turn_v_m_direction == EVerticalMoveDirection::down){
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_right_turn;
                return;
            }
            else{
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_left_turn;
                return;
            }
        }
        else{
            if(turn_h_m_direction == EHorizontalMoveDirection::right && turn_v_m_direction == EVerticalMoveDirection::up){
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_left_turn;
                return;
            }
            else{
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_right_turn;
                return;
            }
        }
    }
    else{
        if(m_vertical_move_direction == EVerticalMoveDirection::up){
            if(turn_h_m_direction == EHorizontalMoveDirection::right && turn_v_m_direction == EVerticalMoveDirection::down){
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_left_turn;
                return;
            }
            else{
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_right_turn;
                return;
            }
        }
        else{
            if(turn_h_m_direction == EHorizontalMoveDirection::left && turn_v_m_direction == EVerticalMoveDirection::down){
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_left_turn;
                return;
            }
            else{
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::taking_right_turn;
                return;
            }
        }
    }
}

void CCar::take_left_turn()
{
    m_is_turning_left = true;

    if(!m_turn_started){
        QPointF car_center_pos = this->pos(); //start point
        car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                                 car_center_pos.y() + this->boundingRect().height()/2);

        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() - 5 - this->boundingRect().height()/2,
                                               m_crossing->pos().y() - this->boundingRect().width()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + m_turn->boundingRect().width() - 5 - this->boundingRect().height()/2,
                                               m_turn->pos().y() - this->boundingRect().width()/2);
                }
            }
            else{
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + 5 + this->boundingRect().height()/2,
                                               m_crossing->pos().y() + m_crossing->boundingRect().height() + this->boundingRect().width()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + 5 + this->boundingRect().height()/2,
                                               m_turn->pos().y() + m_turn->boundingRect().height() + this->boundingRect().width()/2);
                }
            }
        }
        else{
            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() - this->boundingRect().width()/2,
                                               m_crossing->pos().y() + this->boundingRect().height()/2 + 5);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() - this->boundingRect().width()/2,
                                               m_turn->pos().y() + this->boundingRect().height()/2 + 5);
                }
            }
            else{
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() + this->boundingRect().width()/2,
                                               m_crossing->pos().y() + m_crossing->boundingRect().height() - 5 - this->boundingRect().height()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + m_turn->boundingRect().width() + this->boundingRect().width()/2,
                                               m_turn->pos().y() + m_turn->boundingRect().height() - 5 - this->boundingRect().height()/2);
                }
            }
        }

        double radius = abs(car_center_pos.x() - m_turn_end_point.x());
        double arc_length = (2*M_PI*radius)/4;
        m_arc_length_remaining = arc_length;
        m_rotation_remaining = 90;
        m_turn_started = true;
    }

    if(!m_movement_planes_changed){
        if(m_current_movement_plane == EMovementPlane::horizontal){
            m_current_movement_plane = EMovementPlane::vertical;

            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                m_vertical_move_direction = EVerticalMoveDirection::up;
            }
            else{
                m_vertical_move_direction = EVerticalMoveDirection::down;
            }
        }
        else{
            m_current_movement_plane = EMovementPlane::horizontal;

            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                m_horizontal_move_direction = EHorizontalMoveDirection::left;
            }
            else{
                m_horizontal_move_direction = EHorizontalMoveDirection::right;
            }
        }

        m_movement_planes_changed = true;
    }

    int rotation_increments = m_arc_length_remaining/m_current_speed;
    double rotation_increment = m_rotation_remaining/rotation_increments;
    m_arc_length_remaining -= m_current_speed;
    m_rotation_remaining -= rotation_increment;

    this->setRotation(this->rotation() - rotation_increment);

    bool collision_detected = false;
    auto colliding_items = m_map->collidingItems(this);
    for(auto item : colliding_items){
        auto pedestrian = dynamic_cast<CPedestrian*>(item);

        if(pedestrian){
            m_current_speed = 0;
            collision_detected = true;
            update_speed_vector();
        }
    }

    if(!collision_detected){
        double acceleration_possible = calculate_possible_acceleration();
        acceleration_possible = 0.6*acceleration_possible;
        m_current_speed = m_current_speed + acceleration_possible;
        round_current_speed_down_to_limit();
        update_speed_vector();
        QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }
    
    if(m_rotation_remaining < 0.5){
        m_is_turning_right = false;
        m_turn_started = false;
        m_movement_planes_changed = false;

        int target_rotation = 0;
        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                target_rotation = 0;
            }
            else{
                target_rotation = -180;
            }
        }
        else{
            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                target_rotation = -90;
            }
            else{
                target_rotation = 90;
            }
        }

        this->set_rotation_by_center(target_rotation);
        update_speed_vector();
        correct_current_pos_in_regard_to_roadway();

        if(m_was_leaving_a_crossing){
            m_temporary_speed_limit = m_global_speed_limit;
            m_was_leaving_a_crossing = false;
            m_there_is_road_priority_sign_to_check = false;
            m_temporary_speed_limit = m_global_speed_limit;
            m_allowed_horizontal_movements = {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};
            m_allowed_vertical_movements = {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};
        }

        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::riding;
    }
}

void CCar::take_right_turn()
{
    m_is_turning_right = true;

    if(!m_turn_started){
        QPointF car_center_pos = this->pos(); //start point
        car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                                 car_center_pos.y() + this->boundingRect().height()/2);

        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + 5 + this->boundingRect().height(),
                                               m_crossing->pos().y() + m_crossing->boundingRect().height() + this->boundingRect().width()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + 5 + this->boundingRect().height(),
                                               m_turn->pos().y() + m_turn->boundingRect().height() + this->boundingRect().width()/2);
                }
            }
            else{
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() - 10 - this->boundingRect().height()/2,
                                               m_crossing->pos().y() - this->boundingRect().width()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + m_turn->boundingRect().width() - 10 - this->boundingRect().height()/2,
                                               m_turn->pos().y() - this->boundingRect().width()/2);
                }
            }
        }
        else{
            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() + this->boundingRect().width()/2,
                                               m_crossing->pos().y() + m_crossing->boundingRect().height() - this->boundingRect().height()/2 - 5);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() + m_turn->boundingRect().width() + this->boundingRect().width()/2,
                                               m_turn->pos().y() + m_turn->boundingRect().height() - this->boundingRect().height()/2 - 5);
                }
            }
            else{
                if(m_was_leaving_a_crossing){
                    m_turn_end_point = QPointF(m_crossing->pos().x() - this->boundingRect().width()/2,
                                               m_crossing->pos().y() + 5 + this->boundingRect().height()/2);
                }
                else{
                    m_turn_end_point = QPointF(m_turn->pos().x() - this->boundingRect().width()/2,
                                               m_turn->pos().y() + 5 + this->boundingRect().height()/2);
                }
            }
        }

        double radius = abs(car_center_pos.x() - m_turn_end_point.x());
        double arc_length = (2*M_PI*radius)/4;
        m_arc_length_remaining = arc_length;
        m_rotation_remaining = 90;
        m_turn_started = true;
    }

    if(!m_movement_planes_changed){
        if(m_current_movement_plane == EMovementPlane::horizontal){
            m_current_movement_plane = EMovementPlane::vertical;

            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                m_vertical_move_direction = EVerticalMoveDirection::down;
            }
            else{
                m_vertical_move_direction = EVerticalMoveDirection::up;
            }
        }
        else{
            m_current_movement_plane = EMovementPlane::horizontal;

            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                m_horizontal_move_direction = EHorizontalMoveDirection::right;
            }
            else{
                m_horizontal_move_direction = EHorizontalMoveDirection::left;
            }
        }

        m_movement_planes_changed = true;
    }

    int rotation_increments = m_arc_length_remaining/m_current_speed;
    double rotation_increment = m_rotation_remaining/rotation_increments;
    m_arc_length_remaining -= m_current_speed;
    m_rotation_remaining -= rotation_increment;

    this->setRotation(this->rotation() + rotation_increment);

    bool collision_detected = false;
    auto colliding_items = m_map->collidingItems(this);
    for(auto item : colliding_items){
        auto pedestrian = dynamic_cast<CPedestrian*>(item);

        if(pedestrian){
            m_current_speed = 0;
            collision_detected = true;
            update_speed_vector();
        }
    }

    if(!collision_detected){
        double acceleration_possible = calculate_possible_acceleration();
        acceleration_possible = 0.6*acceleration_possible;
        m_current_speed = m_current_speed + acceleration_possible;
        round_current_speed_down_to_limit();
        update_speed_vector();
        QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }

    if(m_rotation_remaining < 0.5){
        m_turn_started = false;
        m_movement_planes_changed = false;
        m_is_turning_right = false;

        int target_rotation = 0;
        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::right){
                target_rotation = 0;
            }
            else{
                target_rotation = -180;
            }
        }
        else{
            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                target_rotation = -90;
            }
            else{
                target_rotation = 90;
            }
        }

        this->set_rotation_by_center(target_rotation);
        update_speed_vector();
        correct_current_pos_in_regard_to_roadway();

        if(m_was_leaving_a_crossing){
            m_temporary_speed_limit = m_global_speed_limit;
            m_was_leaving_a_crossing = false;
            m_there_is_road_priority_sign_to_check = false;
            m_temporary_speed_limit = m_global_speed_limit;
            m_allowed_horizontal_movements = {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};
            m_allowed_vertical_movements = {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};
        }

        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::riding;
    }
}

void CCar::turn_around()
{

    m_current_speed = 0;
    QPointF car_center_pos = this->pos();
    car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                             car_center_pos.y() + this->boundingRect().height()/2);

    CRoadway *current_roadway = nullptr;

    auto items = m_map->items(car_center_pos);
    for(auto item : items){
        auto roadway = dynamic_cast<CRoadway*>(item);

        if(roadway){
            current_roadway = roadway;
            break;
        }
    }

    if(current_roadway == nullptr){
        return;
    }

    bool turning_around_is_possible = check_if_turning_around_is_possible(current_roadway);
    if(!turning_around_is_possible){
        return;
    }

    int roadway_width = (current_roadway->get_carriageways_number()) * (current_roadway->get_lanes_number()) * m_default_cell_size.width();
    int roadside_offset = 5;
    int total_offset = roadway_width - 2*roadside_offset - this->boundingRect().height();

    if(roadway_width == m_default_cell_size.width()){ //Single lane, single carriageway, just rotate
        this->change_rotation_by_center(180);
    }
    else{
        // Move to far right lane

        if(m_current_movement_plane == EMovementPlane::horizontal){
            if(m_horizontal_move_direction == EHorizontalMoveDirection::left){
                setPos(this->pos().x(), this->pos().y() + total_offset);
            }
            else{
                setPos(this->pos().x(), this->pos().y() - total_offset);
            }
        }
        else{
            if(m_vertical_move_direction == EVerticalMoveDirection::up){
                setPos(this->pos().x() - total_offset, this->pos().y());
            }
            else{
                setPos(this->pos().x() + total_offset, this->pos().y());
            }
        }
        this->change_rotation_by_center(180);
    }

    if(m_current_movement_plane == EMovementPlane::horizontal){
        if(m_horizontal_move_direction == EHorizontalMoveDirection::left){
            m_horizontal_move_direction = EHorizontalMoveDirection::right;
        }
        else{
            m_horizontal_move_direction = EHorizontalMoveDirection::left;
        }
    }
    else{
        if(m_vertical_move_direction == EVerticalMoveDirection::up){
            m_vertical_move_direction = EVerticalMoveDirection::down;
        }
        else{
            m_vertical_move_direction = EVerticalMoveDirection::up;
        }
    }

    m_previous_movement_state = m_current_movement_state;
    m_current_movement_state = ECarMovementStates::riding;
}

void CCar::ride_to_crossing_with_lights()
{
    if(!(m_light->is_active())){ //Active->inactive
        m_lights_present_but_inactive = true;
        m_current_movement_state = ECarMovementStates::riding_to_crossing_without_lights;
        return;
    }

    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
    double distance_with_spare_space = distance_to_crossing - 5;

    if(m_light->get_current_lights_phase() != ETrafficLightsPhase::green){
        double deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

        if(deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value && !collision_handled){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 100);
            int sanity_value = dist(gen);

            if(sanity_value >= m_road_users_parameters.m_chance_of_breaking_law){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
                m_is_braking = true;

                if(distance_with_spare_space < 10){
                    m_current_speed = 0;
                    update_speed_vector();
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_with_lights;
                    m_is_waiting = true;
                    m_is_braking = false;
                    return;
                }
            }
            else{
                //Try to slam through, no speed limits for you
                double acceleration_possible = calculate_possible_acceleration();
                m_current_speed = m_current_speed + acceleration_possible;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);

                if(distance_with_spare_space < 10){
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::riding_through_crossing;
                    m_is_waiting = false;
                    m_is_braking = false;
                    return;
                }
            }
        }
        else{
            //Check if you can accelerate some more
            double previous_speed = m_current_speed;
            QPointF previous_pos = this->pos();
            QPointF new_possible_pos;

            if(collision_handled){
                new_possible_pos = this->pos();
            }
            else{
                new_possible_pos = get_new_possible_pos_while_accelerating();
            }

            this->setPos(new_possible_pos);
            double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
            double distance_with_spare_space = distance_to_crossing - 5;
            double new_deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

            if(new_deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value){
                m_current_speed = previous_speed;
                this->setPos(previous_pos);

                if(!collision_handled){
                    m_current_speed = previous_speed - deceleration_needed;
                    update_speed_vector();
                    QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                    this->setPos(new_possible_pos);
                }
            }
            else{
                //If possible, brake
                if(!collision_handled){
                    m_current_speed = m_current_speed - deceleration_needed;
                    update_speed_vector();
                    QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                    this->setPos(new_possible_pos);
                    m_is_braking = true;
                }

                //Stopping is possible, you don't have lights to cross, stop
                if(distance_with_spare_space < 10){
                    m_current_speed = 0;
                    update_speed_vector();
                    m_previous_movement_state = m_current_movement_state;
                    m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_with_lights;
                    m_is_waiting = true;
                    m_is_braking = false;
                    return;
                }
            }
        }
    }
    else{
        if(distance_with_spare_space < 10){
            m_previous_movement_state = m_current_movement_state;
            m_current_movement_state = ECarMovementStates::riding_through_crossing;
            m_is_waiting = false;
            m_is_braking = false;
            return;
        }

        if(collision_handled){
            return;
        }

        double acceleration_possible = calculate_possible_acceleration();
        m_current_speed = m_current_speed + acceleration_possible;
        round_current_speed_down_to_limit();

        update_speed_vector();
        QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }
}

void CCar::ride_to_crossing_without_lights()
{
    if(m_lights_present_but_inactive){
        if(m_light->is_active()){ // Inactive -> active
            m_lights_present_but_inactive = false;
            m_current_movement_state = ECarMovementStates::riding_to_crossing_with_lights;
            return;
        }
    }

    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
    double distance_with_spare_space = distance_to_crossing - 5;
    double deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

    if(deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value && !collision_handled){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 100);
        int sanity_value = dist(gen);

        if(sanity_value >= m_road_users_parameters.m_chance_of_breaking_law){
            m_current_speed = m_current_speed - deceleration_needed;
            update_speed_vector();
            QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
            this->setPos(new_possible_pos);
            m_is_braking = true;

            if(distance_with_spare_space < 10){
                m_current_speed = 0;
                update_speed_vector();
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_without_lights;
                m_is_waiting = true;
                m_is_braking = false;
                return;
            }
        }
        else{
            //Try to slam through, no speed limits for you
            double acceleration_possible = calculate_possible_acceleration();
            m_current_speed = m_current_speed + acceleration_possible;
            update_speed_vector();
            QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
            this->setPos(new_possible_pos);

            if(distance_with_spare_space < 10){
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::riding_through_crossing;
                m_is_waiting = false;
                m_is_braking = false;
                return;
            }
        }
    }
    else{
        //Check if you can accelerate some more
        double previous_speed = m_current_speed;
        QPointF previous_pos = this->pos();
        QPointF new_possible_pos;

        if(collision_handled){
            new_possible_pos = this->pos();
        }
        else{
            new_possible_pos = get_new_possible_pos_while_accelerating();
        }

        this->setPos(new_possible_pos);
        double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
        double distance_with_spare_space = distance_to_crossing - 5;
        double new_deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

        if(new_deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value){
            this->setPos(previous_pos);

            if(!collision_handled){
                m_current_speed = previous_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
            }
        }
        else{
            //If possible, brake
            if(!collision_handled){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
                m_is_braking = true;
            }

            //Stopping is possible, you don't have lights to cross, stop
            if(distance_with_spare_space < 10){
                m_current_speed = 0;
                update_speed_vector();
                m_previous_movement_state = m_current_movement_state;
                m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_without_lights;
                m_is_waiting = true;
                m_is_braking = false;
                return;
            }
        }
    }
}

void CCar::get_closer_to_crossing()
{
    //Do not override previous state, use it to go back after getting closer
    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
    double distance_with_spare_space = distance_to_crossing - 5;
    double deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

    if(deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value && !collision_handled){
        m_current_speed = m_current_speed - deceleration_needed;
        update_speed_vector();
        QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);

        if(distance_with_spare_space < 5){
            m_is_braking = false;
            m_is_waiting = false;
            m_current_movement_state = m_previous_movement_state;
            return;
        }
    }
    else{
        //Check if you can accelerate some more
        double previous_speed = m_current_speed;
        QPointF previous_pos = this->pos();
        QPointF new_possible_pos;

        if(collision_handled){
            new_possible_pos = this->pos();
        }
        else{
            new_possible_pos = get_new_possible_pos_while_accelerating();
        }

        this->setPos(new_possible_pos);
        double distance_to_crossing = get_distance_to_stationary_map_element_in_straight_line(m_crossing);
        double distance_with_spare_space = distance_to_crossing - 5;
        double new_deceleration_needed = (pow(m_current_speed, 2))/(2*distance_with_spare_space);

        if(new_deceleration_needed > m_road_users_parameters_adjusted_to_step.m_max_deceleration_value){
            this->setPos(previous_pos);

            if(!collision_handled){
                m_current_speed = previous_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
            }
        }
        else{
            //If possible, brake
            if(!collision_handled){
                m_current_speed = m_current_speed - deceleration_needed;
                update_speed_vector();
                QPointF new_possible_pos = QPointF(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_possible_pos);
                m_is_braking = true;
            }

            if(distance_with_spare_space < 5){
                m_current_speed = 0;
                update_speed_vector();

                m_current_movement_state = m_previous_movement_state;
                m_is_waiting = true;
                m_is_braking = false;

                return;
            }
        }
    }

}

void CCar::wait_to_cross_crossing_with_lights()
{
    if(m_crossing_after_p_crossing){
        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::getting_closer_to_the_crossing;
        m_crossing_after_p_crossing = false;
    }

    if(!(m_light->is_active())){ //Active->inactive
        m_lights_present_but_inactive = true;
        m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_without_lights;
        return;
    }

    if(m_light->get_current_lights_phase() == ETrafficLightsPhase::green){

        choose_direction_on_crossing_with_lights();
        if(m_current_movement_state == ECarMovementStates::waiting_to_cross_crossing_with_lights){ //Cannot cross?
            m_current_speed = 0;
            update_speed_vector();
        }
        else{
            m_is_waiting = false;
            m_previous_movement_state = ECarMovementStates::waiting_to_cross_crossing_with_lights;
        }
    }
    else{
        m_current_speed = 0;
        m_is_waiting = true;
        update_speed_vector();
    }
}

void CCar::wait_to_cross_crossing_without_lights()
{
    if(m_crossing_after_p_crossing){
        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::getting_closer_to_the_crossing;
        m_crossing_after_p_crossing = false;
    }

    if(m_lights_present_but_inactive){
        if(m_light->is_active()){ // Inactive -> active
            m_lights_present_but_inactive = false;
            m_current_movement_state = ECarMovementStates::waiting_to_cross_crossing_with_lights;
            return;
        }
    }

    choose_direction_on_crossing_without_lights();

    if(m_current_movement_state == ECarMovementStates::waiting_to_cross_crossing_without_lights){ //State not changed, cannot cross
        m_current_speed = 0;
        m_is_waiting = true;
        update_speed_vector();
    }
    else{
        m_previous_movement_state = ECarMovementStates::waiting_to_cross_crossing_without_lights;
        m_is_waiting = false;
    }
}

void CCar::ride_through_crossing()
{
    if(m_riding_through_crossing_offset == 100){
        m_riding_through_crossing_offset = 2*m_crossing->boundingRect().width();
    }

    bool collision_handled = handle_incoming_collisions(15);
    consider_traffic_signs();

    QPointF new_possible_pos;

    if(collision_handled){
        new_possible_pos = this->pos();
    }
    else{
        double acceleration_possible = calculate_possible_acceleration();
        m_current_speed = m_current_speed + acceleration_possible;
        m_current_speed = round_current_speed_down_to_limit();

        update_speed_vector();
        QPointF new_possible_pos(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
        this->setPos(new_possible_pos);
    }

    if(m_riding_through_crossing_offset > 0){
        m_riding_through_crossing_offset -= m_current_speed;
        return;
    }

    bool got_out_of_crossing = true;
    auto items_at_pos = m_map->items(new_possible_pos);

    for(auto item : items_at_pos){
        auto p_crossing = dynamic_cast<CCrossing*>(item);

        if(p_crossing){
            got_out_of_crossing = false;
            break;
        }
    }

    if(got_out_of_crossing){
        m_riding_through_crossing_offset = 100;
        m_previous_movement_state = m_current_movement_state;
        m_temporary_speed_limit = m_global_speed_limit;
        m_was_leaving_a_crossing = false;
        m_there_is_road_priority_sign_to_check = false;

        m_allowed_horizontal_movements = {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};
        m_allowed_vertical_movements = {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};
        m_current_movement_state = ECarMovementStates::riding;
    }
}

bool CCar::handle_incoming_collisions(int spare_space)
{
    QPointF new_possible_pos_while_maintaining_speed(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);

    if(this->get_current_movement_plane() == EMovementPlane::horizontal){

        QPointF new_front_bumper_center_pos;
        QPointF new_bumper_pos_with_spare_space;

        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            new_front_bumper_center_pos = QPointF(new_possible_pos_while_maintaining_speed.x() + this->boundingRect().width(),
                                                  new_possible_pos_while_maintaining_speed.y() + this->boundingRect().height()/2);
            new_bumper_pos_with_spare_space = QPointF(new_front_bumper_center_pos.x() + spare_space, new_front_bumper_center_pos.y());
        }
        else{
            new_front_bumper_center_pos = QPointF(new_possible_pos_while_maintaining_speed.x(),
                                                  new_possible_pos_while_maintaining_speed.y() + this->boundingRect().height()/2);
            new_bumper_pos_with_spare_space = QPointF(new_front_bumper_center_pos.x() - spare_space, new_front_bumper_center_pos.y());
        }

        //First check if there are road users in immediate danger zone
        auto bumper_line = new QGraphicsLineItem(new_front_bumper_center_pos.x(), new_front_bumper_center_pos.y() - this->boundingRect().height()/2,
                                                 new_front_bumper_center_pos.x(), new_front_bumper_center_pos.y() + this->boundingRect().height()/2);
        m_map->addItem(bumper_line);
        auto items_on_crash_course = m_map->collidingItems(bumper_line);
        m_map->removeItem(bumper_line);
        delete bumper_line;

        for(auto item : items_on_crash_course){
            auto road_user = dynamic_cast<CRoadUser*>(item);
            if(road_user && road_user != this){
                //Collision will occur if car maintains speed, slam brakes
                m_current_speed = m_current_speed - m_road_users_parameters_adjusted_to_step.m_max_deceleration_value;
                if(m_current_speed < 0){
                    m_current_speed = 0;
                    update_speed_vector();
                }
                update_speed_vector();
                QPointF new_pos_while_braking(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                QPointF new_bumper_pos_while_braking;

                if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                    new_bumper_pos_while_braking = QPointF(new_pos_while_braking.x() + this->boundingRect().width(),
                                                           new_pos_while_braking.y() + this->boundingRect().height()/2);
                }
                else{
                    new_bumper_pos_while_braking = QPointF(new_pos_while_braking.x(),
                                                           new_pos_while_braking.y() + this->boundingRect().height()/2);
                }

                auto new_bumper_line = new QGraphicsLineItem(new_bumper_pos_while_braking.x(), new_bumper_pos_while_braking.y() - this->boundingRect().height()/2,
                                                             new_bumper_pos_while_braking.x(), new_bumper_pos_while_braking.y() + this->boundingRect().height()/2);

                m_map->addItem(new_bumper_line);
                auto items_still_on_crash_course = m_map->collidingItems(new_bumper_line);
                m_map->removeItem(new_bumper_line);
                delete new_bumper_line;

                //If it does not help,
                for(auto item : items_still_on_crash_course){
                    auto road_user = dynamic_cast<CRoadUser*>(item);
                    if(road_user && road_user != this){
                        //for now "magically" slow down to 0, so there is no movement
                        m_current_speed = 0;
                        update_speed_vector();
                        return true;
                    }
                }
                //If it helped, we are here, move to new pos
                this->setPos(new_pos_while_braking);
                return true;
            }
        }

        //...now check if there are road users in safe zone, but still close in front
        auto bumper_line_with_spare = new QGraphicsLineItem(new_bumper_pos_with_spare_space.x(), new_bumper_pos_with_spare_space.y() - this->boundingRect().height()/2,
                                                            new_bumper_pos_with_spare_space.x(), new_bumper_pos_with_spare_space.y() + this->boundingRect().height()/2);
        m_map->addItem(bumper_line_with_spare);
        auto items_close = m_map->collidingItems(bumper_line_with_spare);
        m_map->removeItem(bumper_line_with_spare);
        delete bumper_line_with_spare;

        for(auto item : items_close){
            auto road_user = dynamic_cast<CRoadUser*>(item);
            if(road_user && road_user != this){
                //Collision will occur if car maintains speed, slam brakes just to be sure
                m_current_speed = m_current_speed - m_road_users_parameters_adjusted_to_step.m_max_deceleration_value;
                if(m_current_speed < 0){
                    m_current_speed = 0;
                    update_speed_vector();
                }
                update_speed_vector();
                QPointF new_pos_while_braking(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_pos_while_braking);
                return true;
            }
        }
    }
    else{
        QPointF new_front_bumper_center_pos;
        QPointF new_bumper_pos_with_spare_space;
        QPointF bounding_rect_left_corner_mapped = QPointF(new_possible_pos_while_maintaining_speed.x() + this->boundingRect().width()/2 - this->boundingRect().height()/2,
                                                           new_possible_pos_while_maintaining_speed.y() - this->boundingRect().width()/2 + this->boundingRect().height()/2);

        if(this->get_vertical_move_direction() == EVerticalMoveDirection::down){
            new_front_bumper_center_pos = QPointF(bounding_rect_left_corner_mapped.x() + this->boundingRect().height()/2,
                                                  bounding_rect_left_corner_mapped.y() + this->boundingRect().width());
            new_bumper_pos_with_spare_space = QPointF(new_front_bumper_center_pos.x(), new_front_bumper_center_pos.y() + spare_space);
        }
        else{
            new_front_bumper_center_pos = QPointF(bounding_rect_left_corner_mapped.x() + this->boundingRect().height()/2,
                                                  bounding_rect_left_corner_mapped.y());
            new_bumper_pos_with_spare_space = QPointF(new_front_bumper_center_pos.x(), new_front_bumper_center_pos.y() - spare_space);
        }

        //First check if there are road users in immediate danger zone
        auto bumper_line = new QGraphicsLineItem(new_front_bumper_center_pos.x() - this->boundingRect().height()/2, new_front_bumper_center_pos.y(),
                                                 new_front_bumper_center_pos.x() + this->boundingRect().height()/2, new_front_bumper_center_pos.y());
        m_map->addItem(bumper_line);
        auto items_on_crash_course = m_map->collidingItems(bumper_line);
        m_map->removeItem(bumper_line);
        delete bumper_line;

        for(auto item : items_on_crash_course){
            auto road_user = dynamic_cast<CRoadUser*>(item);
            if(road_user && road_user != this){
                //Collision will occur if car maintains speed, slam brakes
                m_current_speed = m_current_speed - m_road_users_parameters_adjusted_to_step.m_max_deceleration_value;
                if(m_current_speed < 0){
                    m_current_speed = 0;
                }
                update_speed_vector();
                QPointF new_pos_while_braking(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                QPointF new_bounding_rect_left_corner_mapped = QPointF(new_pos_while_braking.x() + this->boundingRect().width()/2 - this->boundingRect().height()/2,
                                                                   new_pos_while_braking.y() - this->boundingRect().width()/2 + this->boundingRect().height()/2);
                QPointF new_bumper_pos_while_braking;

                if(this->get_vertical_move_direction() == EVerticalMoveDirection::down){
                    new_bumper_pos_while_braking = QPointF(new_bounding_rect_left_corner_mapped.x() + this->boundingRect().height()/2,
                                                          new_bounding_rect_left_corner_mapped.y() + this->boundingRect().width());
                }
                else{
                    new_bumper_pos_while_braking = QPointF(new_bounding_rect_left_corner_mapped.x() + this->boundingRect().height()/2,
                                                          new_bounding_rect_left_corner_mapped.y());
                }

                auto new_bumper_line =  new QGraphicsLineItem(new_bumper_pos_while_braking.x() - this->boundingRect().height()/2, new_bumper_pos_while_braking.y(),
                                        new_bumper_pos_while_braking.x() + this->boundingRect().height()/2, new_bumper_pos_while_braking.y());

                m_map->addItem(new_bumper_line);
                auto items_still_on_crash_course = m_map->collidingItems(new_bumper_line);
                m_map->removeItem(new_bumper_line);
                delete new_bumper_line;

                //If it does not help,
                for(auto item : items_still_on_crash_course){
                    auto road_user = dynamic_cast<CRoadUser*>(item);
                    if(road_user && road_user != this){
                        //for now "magically" slow down to 0, so there is no movement
                        m_current_speed = 0;
                        update_speed_vector();
                        return true;
                    }
                }
                //If it helped, we are here, move to new pos
                this->setPos(new_pos_while_braking);
                return true;
            }
        }

        //...now check if there are road users in safe zone, but still close in front
        auto bumper_line_with_spare = new QGraphicsLineItem(new_bumper_pos_with_spare_space.x() - this->boundingRect().height()/2, new_bumper_pos_with_spare_space.y(),
                                                            new_bumper_pos_with_spare_space.x() + this->boundingRect().height()/2, new_bumper_pos_with_spare_space.y());
        m_map->addItem(bumper_line_with_spare);
        auto items_close = m_map->collidingItems(bumper_line_with_spare);
        m_map->removeItem(bumper_line_with_spare);
        delete bumper_line_with_spare;

        for(auto item : items_close){
            auto road_user = dynamic_cast<CRoadUser*>(item);
            if(road_user && road_user != this){
                //Collision will occur if car maintains speed, slam brakes just to be sure
                m_current_speed = m_current_speed - m_road_users_parameters_adjusted_to_step.m_max_deceleration_value;
                if(m_current_speed < 0){
                    m_current_speed = 0;
                }
                update_speed_vector();
                QPointF new_pos_while_braking(this->pos().x() + m_speed_vector.first, this->pos().y() + m_speed_vector.second);
                this->setPos(new_pos_while_braking);
                return true;
            }
        }
    }

    return false;
}

double CCar::calculate_possible_acceleration()
{
    /**
     * Acceleration looks something like that
     *
     * 100% - m_current_speed = 50 _______________ 0.5*max_speed
     *       /                                                  \
     *      /                                                     \
     *     /                                                        \
     *    /                                                           \
     *   /                                                              \
     *  /                                                                 \
     *30%                                                                   \
     *20%                                                                     \
     *10%                                                                       \
     * 0%                                                                         \ max_speed reached
     */
    double speed_reserve = m_road_users_parameters_adjusted_to_step.m_max_speed - m_current_speed;
    double acceleration_possible = 0;

    if(speed_reserve == 0){
        acceleration_possible = 0;
    }
    else if(m_current_speed < normal_speed_to_simulation_speed(50)){

        double base_starting_acceleration = 0.3*m_road_users_parameters_adjusted_to_step.m_max_acceleration_value;
        double acceleration_growth = 0;

        if(m_current_speed != 0){
            acceleration_growth = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value *
                                  (m_current_speed/normal_speed_to_simulation_speed(50))*0.7;
        }
        acceleration_possible = base_starting_acceleration + acceleration_growth;
    }
    else if(speed_reserve > 0.5*m_road_users_parameters_adjusted_to_step.m_max_speed){
        acceleration_possible = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value;
    }
    else{
        acceleration_possible = m_road_users_parameters_adjusted_to_step.m_max_acceleration_value *
                                (speed_reserve/m_road_users_parameters_adjusted_to_step.m_max_speed);
    }

    return acceleration_possible;
}

bool CCar::check_for_p_crossing_lights()
{
    bool is_crossing_with_lights = false;

    int x_offset = 5;
    int y_offset = 5;

    EMovementPlane pc_movement_plane = m_p_crossing->get_movement_plane();
    QPoint pavement_center_pos;

    // Check for lights on the RIGHT SIDE FROM THE CAR'S PERSPECTIVE
    if(pc_movement_plane == EMovementPlane::horizontal){ //Pedestrian crossing orientation horizontal <=> car is moving vertically
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            pavement_center_pos = QPoint(m_p_crossing->pos().x() + 1.5*m_default_cell_size.width(),
                                        m_p_crossing->pos().y() + m_default_cell_size.height()/2);
        }
        else{
            pavement_center_pos = QPoint(m_p_crossing->pos().x() - m_default_cell_size.width()/2,
                                         m_p_crossing->pos().y() + m_default_cell_size.height()/2);
        }
    }
    else{ //Pedestrian crossing orientation vertical <=> car is moving horizontally
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            pavement_center_pos = QPoint(m_p_crossing->pos().x() + m_default_cell_size.width()/2,
                                         m_p_crossing->pos().y() + 1.5*m_default_cell_size.height());
        }
        else{
            pavement_center_pos = QPoint(m_p_crossing->pos().x() + m_default_cell_size.width()/2,
                                         m_p_crossing->pos().y() - m_default_cell_size.height()/2);
        }
    }

    CPavement *pavement = nullptr;
    auto items_at_pavement_pos = m_map->items(pavement_center_pos);

    for(auto item : items_at_pavement_pos){
        auto pavement_item = dynamic_cast<CPavement*>(item);
        if(pavement_item){
            pavement = pavement_item;
            break;
        }
    }

    if(!pavement){ //It shouldn't happen. Pavement following pedestrian crossing should be asserted by map creation validation
        return false;
    }

    QPoint possible_lights_pos = QPoint(pavement->pos().x() + x_offset, pavement->pos().y() + y_offset);

    auto items_at_lights_pos = m_map->items(possible_lights_pos);
    CTrafficLight *traffic_light = nullptr;

    for(auto item : items_at_lights_pos){
        auto tl = dynamic_cast<CTrafficLight*>(item);
        if(tl){
            traffic_light = tl;

            if(tl->is_active()){
                is_crossing_with_lights = true;
            }
            else{
                m_lights_present_but_inactive = true;
            }

            break;
        }
    }

    if(is_crossing_with_lights){
        m_light = traffic_light;
        return true;
    }
    else{
        return false;
    }
}

bool CCar::check_for_crossing_lights()
{
    bool is_crossing_with_lights = false;

    int x_offset = 5;
    int y_offset = 5;

    QPoint crossing_light_pos;
    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            crossing_light_pos = QPoint(m_crossing->pos().x() - m_default_cell_size.width() + x_offset,
                                        m_crossing->pos().y() + m_crossing->boundingRect().height() + y_offset);
        }
        else{
            crossing_light_pos = QPoint(m_crossing->pos().x() + m_crossing->boundingRect().width() + x_offset,
                                        m_crossing->pos().y() - m_default_cell_size.height() + y_offset);
        }
    }
    else{
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            crossing_light_pos = QPoint(m_crossing->pos().x() + m_crossing->boundingRect().width() + x_offset,
                                        m_crossing->pos().y() + m_crossing->boundingRect().height() + y_offset);
        }
        else{
            crossing_light_pos = QPoint(m_crossing->pos().x() - m_default_cell_size.width() + x_offset,
                                        m_crossing->pos().y() - m_default_cell_size.height() + y_offset);
        }
    }

    auto items_at_lights_pos = m_map->items(crossing_light_pos);
    CTrafficLight *traffic_light = nullptr;

    for(auto item : items_at_lights_pos){
        auto tl = dynamic_cast<CTrafficLight*>(item);
        if(tl){
            traffic_light = tl;

            if(tl->is_active()){
                is_crossing_with_lights = true;
            }
            else{
                m_lights_present_but_inactive = true;
            }

            break;
        }
    }

    if(is_crossing_with_lights){
        m_light = traffic_light;
        return true;
    }
    else{
        return false;
    }
}

bool CCar::check_for_pedestrians_trying_to_cross()
{
    auto pedestrians_crossing = m_map->collidingItems(m_p_crossing);
    for(auto item : pedestrians_crossing){
        auto pedestrian = dynamic_cast<CPedestrian*>(item);
        if(pedestrian){
            return false;
        }
    }

    if(m_p_crossing->get_movement_plane() == EMovementPlane::horizontal){ //Car moving vertically, calculate by y axis
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){

            QPointF right_pavement_pos = QPointF(m_p_crossing->pos().x() + 1.5*m_default_cell_size.width(),
                                                 m_p_crossing->pos().y() + 0.5*m_default_cell_size.height());

            auto items_at_right_pavement_pos = m_map->items(right_pavement_pos);
            for(auto item : items_at_right_pavement_pos){
                auto pavement = dynamic_cast<CPavement*>(item);
                if(pavement){
                    auto pavement_colliding_items = m_map->collidingItems(pavement);

                    for(auto pavement_item : pavement_colliding_items){

                        auto pedestrian = dynamic_cast<CPedestrian*>(pavement_item);
                        if(pedestrian && pedestrian->is_waiting_to_pass()){
                            return true;
                        }
                    }
                    break;
                }
            }

            bool is_pavement = false;

            QPoint left_pavement_center_pos = QPoint(m_p_crossing->pos().x() - m_default_cell_size.width()/2,
                                                     m_p_crossing->pos().y() + m_default_cell_size.height()/2);

            while(!is_pavement){
                auto items_at_pavement_pos = m_map->items(left_pavement_center_pos);

                for(auto item : items_at_pavement_pos){

                    auto left_pavement = dynamic_cast<CPavement*>(item);
                    auto left_p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

                    if(left_p_crossing){

                        auto left_p_crossing_colliding_items = m_map->collidingItems(left_p_crossing);
                        for(auto left_p_crossing_item : left_p_crossing_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(left_p_crossing_item);
                            if(pedestrian){
                                return true;
                            }
                        }
                        break;
                    }

                    if(left_pavement){
                        is_pavement = true;
                        auto left_pavement_colliding_items = m_map->collidingItems(left_pavement);

                        for(auto left_pavement_item : left_pavement_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(left_pavement_item);
                            if(pedestrian && pedestrian->is_waiting_to_pass()){
                                return true;
                            }
                        }
                        break;
                    }
                }

                if(!is_pavement){
                    left_pavement_center_pos = QPoint(left_pavement_center_pos.x() - m_default_cell_size.width(),
                                                      left_pavement_center_pos.y());
                }
            }

        }
        else{

            QPointF left_pavement_pos = QPointF(m_p_crossing->pos().x() - 0.5*m_default_cell_size.width(),
                                                m_p_crossing->pos().y() + 0.5*m_default_cell_size.height());

            auto items_at_left_pavement_pos = m_map->items(left_pavement_pos);
            for(auto item : items_at_left_pavement_pos){
                auto pavement = dynamic_cast<CPavement*>(item);
                if(pavement){
                    auto pavement_colliding_items = m_map->collidingItems(pavement);

                    for(auto pavement_item : pavement_colliding_items){

                        auto pedestrian = dynamic_cast<CPedestrian*>(pavement_item);
                        if(pedestrian && pedestrian->is_waiting_to_pass()){
                            return true;
                        }
                    }
                    break;
                }
            }

            bool is_pavement = false;

            QPoint right_pavement_center_pos = QPoint(m_p_crossing->pos().x() + 1.5*m_default_cell_size.width(),
                                                      m_p_crossing->pos().y() + m_default_cell_size.height()/2);

            while(!is_pavement){
                auto items_at_pavement_pos = m_map->items(right_pavement_center_pos);

                for(auto item : items_at_pavement_pos){

                    auto right_p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

                    if(right_p_crossing){

                        auto right_p_crossing_colliding_items = m_map->collidingItems(right_p_crossing);
                        for(auto left_p_crossing_item : right_p_crossing_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(left_p_crossing_item);
                            if(pedestrian){
                                return true;
                            }
                        }
                        break;
                    }

                    auto right_pavement = dynamic_cast<CPavement*>(item);

                    if(right_pavement){
                        is_pavement = true;
                        auto right_pavement_colliding_items = m_map->collidingItems(right_pavement);

                        for(auto right_pavement_item : right_pavement_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(right_pavement_item);
                            if(pedestrian && pedestrian->is_waiting_to_pass()){
                                return true;
                            }
                        }
                        break;
                    }
                }

                if(!is_pavement){
                    right_pavement_center_pos = QPoint(right_pavement_center_pos.x() + m_default_cell_size.width(),
                                                       right_pavement_center_pos.y());
                }
            }
        }
    }
    else{ //Car moving horizontally, calculate by x axis
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){

            QPointF left_pavement_pos = QPointF(m_p_crossing->pos().x() + 0.5*m_default_cell_size.width(),
                                                m_p_crossing->pos().y() + 1.5*m_default_cell_size.height());

            auto items_at_left_pavement_pos = m_map->items(left_pavement_pos);
            for(auto item : items_at_left_pavement_pos){
                auto pavement = dynamic_cast<CPavement*>(item);
                if(pavement){
                    auto pavement_colliding_items = m_map->collidingItems(pavement);

                    for(auto pavement_item : pavement_colliding_items){

                        auto pedestrian = dynamic_cast<CPedestrian*>(pavement_item);
                        if(pedestrian && pedestrian->is_waiting_to_pass()){
                            return true;
                        }
                    }
                    break;
                }
            }

            bool is_pavement = false;

            QPoint right_pavement_center_pos = QPoint(m_p_crossing->pos().x() + 0.5*m_default_cell_size.width(),
                                                      m_p_crossing->pos().y() - m_default_cell_size.height()/2);

            while(!is_pavement){
                auto items_at_pavement_pos = m_map->items(right_pavement_center_pos);

                for(auto item : items_at_pavement_pos){

                    auto right_p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

                    if(right_p_crossing){

                        auto right_p_crossing_colliding_items = m_map->collidingItems(right_p_crossing);
                        for(auto left_p_crossing_item : right_p_crossing_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(left_p_crossing_item);
                            if(pedestrian){
                                return true;
                            }
                        }
                        break;
                    }

                    auto right_pavement = dynamic_cast<CPavement*>(item);

                    if(right_pavement){
                        is_pavement = true;
                        auto right_pavement_colliding_items = m_map->collidingItems(right_pavement);

                        for(auto right_pavement_item : right_pavement_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(right_pavement_item);
                            if(pedestrian && pedestrian->is_waiting_to_pass()){
                                return true;
                            }
                        }
                        break;
                    }
                }

                if(!is_pavement){
                    right_pavement_center_pos = QPoint(right_pavement_center_pos.x(),
                                                       right_pavement_center_pos.y() - m_default_cell_size.height());
                }
            }

        }
        else{

            QPointF left_pavement_pos = QPointF(m_p_crossing->pos().x() + 0.5*m_default_cell_size.width(),
                                                m_p_crossing->pos().y() - 0.5*m_default_cell_size.height());

            auto items_at_left_pavement_pos = m_map->items(left_pavement_pos);
            for(auto item : items_at_left_pavement_pos){
                auto pavement = dynamic_cast<CPavement*>(item);
                if(pavement){
                    auto pavement_colliding_items = m_map->collidingItems(pavement);

                    for(auto pavement_item : pavement_colliding_items){

                        auto pedestrian = dynamic_cast<CPedestrian*>(pavement_item);
                        if(pedestrian && pedestrian->is_waiting_to_pass()){
                            return true;
                        }
                    }
                    break;
                }
            }

            bool is_pavement = false;

            QPoint right_pavement_center_pos = QPoint(m_p_crossing->pos().x() + 0.5*m_default_cell_size.width(),
                                                      m_p_crossing->pos().y() + 1.5*m_default_cell_size.height());

            while(!is_pavement){
                auto items_at_pavement_pos = m_map->items(right_pavement_center_pos);

                for(auto item : items_at_pavement_pos){

                    auto right_p_crossing = dynamic_cast<CPedestrianCrossing*>(item);

                    if(right_p_crossing){

                        auto right_p_crossing_colliding_items = m_map->collidingItems(right_p_crossing);
                        for(auto left_p_crossing_item : right_p_crossing_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(left_p_crossing_item);
                            if(pedestrian){
                                return true;
                            }
                        }
                        break;
                    }

                    auto right_pavement = dynamic_cast<CPavement*>(item);

                    if(right_pavement){
                        is_pavement = true;
                        auto right_pavement_colliding_items = m_map->collidingItems(right_pavement);

                        for(auto right_pavement_item : right_pavement_colliding_items){

                            auto pedestrian = dynamic_cast<CPedestrian*>(right_pavement_item);
                            if(pedestrian && pedestrian->is_waiting_to_pass()){
                                return true;
                            }
                        }
                        break;
                    }
                }

                if(!is_pavement){
                    right_pavement_center_pos = QPoint(right_pavement_center_pos.x(),
                                                       right_pavement_center_pos.y() + m_default_cell_size.height());
                }
            }
        }
    }

    return false;
}

void CCar::consider_traffic_signs()
{
    //Same situation as with lights, get the sign on the right side of the car -
    //but this time check next possible cell, not currently adjacent one
    int x_offset = 5;
    int y_offset = 5;

    QPoint next_possible_traffic_sign_substrate_pos;
    QPointF car_center_pos = this->pos();
    car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                             car_center_pos.y() + this->boundingRect().height()/2);

    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            next_possible_traffic_sign_substrate_pos = QPoint(car_center_pos.x() + m_default_cell_size.width(),
                                        car_center_pos.y() + m_default_cell_size.height());
        }
        else{
            next_possible_traffic_sign_substrate_pos = QPoint(car_center_pos.x() - m_default_cell_size.width(),
                                                              car_center_pos.y() + m_default_cell_size.height());
        }
    }
    else{
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            next_possible_traffic_sign_substrate_pos = QPoint(car_center_pos.x() + m_default_cell_size.width(),
                                                              car_center_pos.y() - m_default_cell_size.height());
        }
        else{
            next_possible_traffic_sign_substrate_pos = QPoint(car_center_pos.x() - m_default_cell_size.width(),
                                                              car_center_pos.y() + m_default_cell_size.height());
        }
    }

    auto possibly_signs_substrates = m_map->items(next_possible_traffic_sign_substrate_pos);
    bool possibly_signs_substrate = false;

    CFiller *filler = nullptr;
    CPavement *pavement = nullptr;

    for(auto item : possibly_signs_substrates){
        auto map_element = dynamic_cast<CStationaryMapElement*>(item);
        if(!map_element){
            continue;
        }

        if(map_element->get_map_element_type() == EStationaryMapElementType::filler){
            filler = dynamic_cast<CFiller*>(map_element);
            possibly_signs_substrate = true;
            break;
        }
        else{
            pavement = dynamic_cast<CPavement*>(map_element);
            if(pavement){
                possibly_signs_substrate = true;
                break;
            }
        }
    }

    if(!possibly_signs_substrate){
        return;
    }

    QPointF corner_pos;
    if(filler){
        corner_pos = filler->pos();
    }
    else if(pavement){
        corner_pos = pavement->pos();
    }
    else{
        return;
    }

    auto items_at_traffic_signs_pos = m_map->items(QPointF(corner_pos.x() + x_offset, corner_pos.y() + y_offset));
    CTrafficSign *traffic_sign = nullptr;

    for(auto item : items_at_traffic_signs_pos){
        auto ts = dynamic_cast<CTrafficSign*>(item);
        if(ts){
            traffic_sign = ts;
            break;
        }
    }


    if(traffic_sign && traffic_sign != m_last_traffic_sign){
        consider_traffic_sign(traffic_sign);
    }
}

void CCar::consider_traffic_sign(CTrafficSign *traffic_sign)
{
    m_last_traffic_sign = traffic_sign;

    if(traffic_sign->get_traffic_sign_category() == ETrafficSignCategory::information_sign){

        if(traffic_sign->get_traffic_sign_type_as_int() == EInformationSigns::built_up_area_sign){
            m_global_speed_limit = normal_speed_to_simulation_speed(50);
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EInformationSigns::non_built_up_area_sign){
            m_global_speed_limit = normal_speed_to_simulation_speed(90);
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EInformationSigns::priority_road){
            m_there_is_road_priority_sign_to_check = true;
            m_current_road_priority_sign = std::pair<ETrafficSignCategory, int>
                (ETrafficSignCategory::information_sign, EInformationSigns::priority_road);
        }
    }
    else if(traffic_sign->get_traffic_sign_category() == ETrafficSignCategory::injuction_sign){

        if(traffic_sign->get_traffic_sign_type_as_int() == EInjuctionSigns::drive_straight){
            for(int i = 0; i < m_allowed_horizontal_movements.size(); i++){
                m_allowed_horizontal_movements[i].second = false;
            }

            for(int i = 0; i < m_allowed_vertical_movements.size(); i++){
                m_allowed_vertical_movements[i].second = true;
            }
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EInjuctionSigns::turn_left){
            for(int i = 0; i < m_allowed_vertical_movements.size(); i++){
                m_allowed_vertical_movements[i].second = false;
            }

            m_allowed_horizontal_movements[0].second = true;
            m_allowed_horizontal_movements[1].second = false;
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EInjuctionSigns::turn_right){
            for(int i = 0; i < m_allowed_vertical_movements.size(); i++){
                m_allowed_vertical_movements[i].second = false;
            }

            m_allowed_horizontal_movements[0].second = false;
            m_allowed_horizontal_movements[1].second = true;
        }
    }
    else if(traffic_sign->get_traffic_sign_category() == ETrafficSignCategory::prohibition_sign){
        if(traffic_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::speed_limit){
            auto speed_limit_sign = static_cast<CProhibitionSign*>(traffic_sign);
            m_temporary_speed_limit = normal_speed_to_simulation_speed(speed_limit_sign->get_speed_limit());
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::stop){
            m_there_is_road_priority_sign_to_check = true;
            m_current_road_priority_sign = std::pair<ETrafficSignCategory, int>
                (ETrafficSignCategory::prohibition_sign, EProhibitionSigns::stop);
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::left_turn_prohibited){
            m_allowed_horizontal_movements[0].second = false;
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::right_turn_prohibited){
            m_allowed_horizontal_movements[0].second = false;
        }
        // NO ENTRY SIGN WILL BE CHECKED IN THE CROSSINGS MOVEMENT LOGIC
    }
    else if(traffic_sign->get_traffic_sign_category() == ETrafficSignCategory::warning_sign){

        if(traffic_sign->get_traffic_sign_type_as_int() == EWarningSigns::equivalent_intersection){
            m_there_is_road_priority_sign_to_check = false;
            m_current_road_priority_sign = std::pair<ETrafficSignCategory, int>
                (ETrafficSignCategory::warning_sign, EWarningSigns::equivalent_intersection);
        }
        else if(traffic_sign->get_traffic_sign_type_as_int() == EWarningSigns::give_way){
            m_there_is_road_priority_sign_to_check = true;
            m_current_road_priority_sign = std::pair<ETrafficSignCategory, int>
                (ETrafficSignCategory::warning_sign, EWarningSigns::give_way);
        }
    }
}

QPointF CCar::get_current_cell_pos()
{
    int x_pos_rounded_down_to_grid = (int)((this->pos().x() + this->boundingRect().width()/2)/(int)m_default_cell_size.width())*m_default_cell_size.width();
    int y_pos_rounded_down_to_grid = (int)((this->pos().y() + this->boundingRect().height()/2)/(int)m_default_cell_size.height())*m_default_cell_size.height();

    return QPointF(x_pos_rounded_down_to_grid, y_pos_rounded_down_to_grid);
}

double CCar::round_current_speed_down_to_limit()
{
    int max_limit = m_global_speed_limit;
    if(m_temporary_speed_limit != -1){
        max_limit = m_temporary_speed_limit;
    }

    if(m_current_speed > max_limit){
        m_current_speed = max_limit;
    }

    return m_current_speed;
}

void CCar::choose_direction_on_crossing_without_lights()
{
    m_was_leaving_a_crossing = true;
    QVector<int> allowed_directions = get_allowed_crossing_directions();

    if(allowed_directions.isEmpty()){
        if(m_has_designated_destination){
            destination_reached();
        }

        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::turning_around;
    }

    int destination_direction = -1;

    if(m_has_designated_destination){
        destination_direction = find_crossing_side_to_destination(allowed_directions);
        if(destination_direction == -1){
            destination_reached(); //Destination lost in reality
        }
    }

    int direction_chosen = 0;
    if(destination_direction != -1){
        direction_chosen = destination_direction;
    }
    else{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, allowed_directions.size() - 1); //No turning around on crossings for now
        int direction_it = dist(gen);
        direction_chosen = allowed_directions[direction_it];
    }

    //It could be simplified, 90 degress of rotation <=> shift side's value by one
    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            if(direction_chosen == 1){

                if(check_if_driving_straight_through_crossing_without_lights_is_possible()){
                    m_current_movement_state = ECarMovementStates::riding_through_crossing;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 0){
                if(check_if_driving_left_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 2){
                if(check_if_driving_right_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_right_turn;
                }
                else{
                    return;
                }
            }
        }
        else{
            if(direction_chosen == 3){
                if(check_if_driving_straight_through_crossing_without_lights_is_possible()){
                    m_current_movement_state = ECarMovementStates::riding_through_crossing;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 2){
                if(check_if_driving_left_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 0){
                if(check_if_driving_right_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_right_turn;
                }
                else{
                    return;
                }
            }
        }
    }
    else{
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            if(direction_chosen == 0){
                if(check_if_driving_straight_through_crossing_without_lights_is_possible()){
                    m_current_movement_state = ECarMovementStates::riding_through_crossing;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 3){
                if(check_if_driving_left_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 1){
                if(check_if_driving_right_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_right_turn;
                }
                else{
                    return;
                }
            }
        }
        else{
            if(direction_chosen == 2){
                if(check_if_driving_straight_through_crossing_without_lights_is_possible()){
                    m_current_movement_state = ECarMovementStates::riding_through_crossing;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 1){
                if(check_if_driving_left_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 3){
                if(check_if_driving_right_through_crossing_without_lights_is_possible()){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_right_turn;
                }
                else{
                    return;
                }
            }
        }
    }

    return;
}

int CCar::find_crossing_side_to_destination(QVector<int> allowed_directions)
{
    int destination_direction = -1;
    int crossing_side_lenght = m_crossing->boundingRect().width();

    for(int i = 0; i < allowed_directions.size(); i++){

        if(allowed_directions[i] == 0){
            QGraphicsLineItem *check_line = new QGraphicsLineItem(m_crossing->pos().x() + 5,
                                                                  m_crossing->pos().y() - 5,
                                                                  m_crossing->pos().x() + crossing_side_lenght - 5,
                                                                  m_crossing->pos().y() - 5);
            m_map->addItem(check_line);
            auto colliding_items = m_map->collidingItems(check_line);
            m_map->removeItem(check_line);
            delete check_line;

            for(auto item : colliding_items){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element){
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::pavement){
                    continue;
                }


                if(crossing_side_lenght == m_default_cell_size.width()){
                    QPoint center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                               road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint pos_mapped_to_grid = map_point_to_grid_point(center_pos);

                    if(std::find(m_path_to_destination.begin(), m_path_to_destination.end(), pos_mapped_to_grid) != m_path_to_destination.end()){

                        destination_direction = 0;
                        return destination_direction;
                    }
                }
                else{
                    QPoint left_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                         road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint left_pos_mapped_to_grid = map_point_to_grid_point(left_half_center_pos);

                    QPoint right_half_center_pos = QPoint(road_element->pos().x() + 1.5*m_default_cell_size.width(),
                                                          road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint right_pos_mapped_to_grid = map_point_to_grid_point(right_half_center_pos);

                    if((std::find(m_path_to_destination.begin(), m_path_to_destination.end(), left_pos_mapped_to_grid) != m_path_to_destination.end())
                        || (std::find(m_path_to_destination.begin(), m_path_to_destination.end(), right_pos_mapped_to_grid) != m_path_to_destination.end())){
                        destination_direction = 0;
                        return destination_direction;
                    }
                }
            }
        }
        else if(allowed_directions[i] == 1){

            QGraphicsLineItem *check_line = new QGraphicsLineItem(m_crossing->pos().x() + crossing_side_lenght + 5,
                                                                  m_crossing->pos().y() + 5,
                                                                  m_crossing->pos().x() + crossing_side_lenght + 5,
                                                                  m_crossing->pos().y() + crossing_side_lenght - 5);
            m_map->addItem(check_line);
            auto colliding_items = m_map->collidingItems(check_line);
            m_map->removeItem(check_line);
            delete check_line;

            for(auto item : colliding_items){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element){
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::pavement){
                    continue;
                }


                if(crossing_side_lenght == m_default_cell_size.width()){
                    QPoint center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                               road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint pos_mapped_to_grid = map_point_to_grid_point(center_pos);

                    if(std::find(m_path_to_destination.begin(), m_path_to_destination.end(), pos_mapped_to_grid) != m_path_to_destination.end()){

                        destination_direction = 1;
                        return destination_direction;
                    }
                }
                else{
                    QPoint left_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                         road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint left_pos_mapped_to_grid = map_point_to_grid_point(left_half_center_pos);

                    QPoint right_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                          road_element->pos().y() + 1.5*m_default_cell_size.height());
                    QPoint right_pos_mapped_to_grid = map_point_to_grid_point(right_half_center_pos);

                    if((std::find(m_path_to_destination.begin(), m_path_to_destination.end(), left_pos_mapped_to_grid) != m_path_to_destination.end())
                        || (std::find(m_path_to_destination.begin(), m_path_to_destination.end(), right_pos_mapped_to_grid) != m_path_to_destination.end())){
                        destination_direction = 1;
                        return destination_direction;
                    }
                }
            }

        }
        else if(allowed_directions[i] == 2){

            QGraphicsLineItem *check_line = new QGraphicsLineItem(m_crossing->pos().x() + 5,
                                                                  m_crossing->pos().y() + crossing_side_lenght + 5,
                                                                  m_crossing->pos().x() + crossing_side_lenght - 5,
                                                                  m_crossing->pos().y() + crossing_side_lenght + 5);
            m_map->addItem(check_line);
            auto colliding_items = m_map->collidingItems(check_line);
            m_map->removeItem(check_line);
            delete check_line;

            for(auto item : colliding_items){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element){
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::pavement){
                    continue;
                }


                if(crossing_side_lenght == m_default_cell_size.width()){
                    QPoint center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                               road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint pos_mapped_to_grid = map_point_to_grid_point(center_pos);

                    if(std::find(m_path_to_destination.begin(), m_path_to_destination.end(), pos_mapped_to_grid) != m_path_to_destination.end()){

                        destination_direction = 2;
                        return destination_direction;
                    }
                }
                else{
                    QPoint left_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                         road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint left_pos_mapped_to_grid = map_point_to_grid_point(left_half_center_pos);

                    QPoint right_half_center_pos = QPoint(road_element->pos().x() + 1.5*m_default_cell_size.width(),
                                                          road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint right_pos_mapped_to_grid = map_point_to_grid_point(right_half_center_pos);

                    if((std::find(m_path_to_destination.begin(), m_path_to_destination.end(), left_pos_mapped_to_grid) != m_path_to_destination.end())
                        || (std::find(m_path_to_destination.begin(), m_path_to_destination.end(), right_pos_mapped_to_grid) != m_path_to_destination.end())){
                        destination_direction = 2;
                        return destination_direction;
                    }
                }
            }
        }
        else if(allowed_directions[i] == 3){

            QGraphicsLineItem *check_line = new QGraphicsLineItem(m_crossing->pos().x() - 5,
                                                                  m_crossing->pos().y() + 5,
                                                                  m_crossing->pos().x() - 5,
                                                                  m_crossing->pos().y() + crossing_side_lenght - 5);
            m_map->addItem(check_line);
            auto colliding_items = m_map->collidingItems(check_line);
            m_map->removeItem(check_line);
            delete check_line;

            for(auto item : colliding_items){
                auto road_element = dynamic_cast<CRoadElement*>(item);
                if(!road_element){
                    continue;
                }

                if(road_element->get_road_element_type() == ERoadElementType::pavement){
                    continue;
                }


                if(crossing_side_lenght == m_default_cell_size.width()){
                    QPoint center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                               road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint pos_mapped_to_grid = map_point_to_grid_point(center_pos);

                    if(std::find(m_path_to_destination.begin(), m_path_to_destination.end(), pos_mapped_to_grid) != m_path_to_destination.end()){

                        destination_direction = 3;
                        return destination_direction;
                    }
                }
                else{
                    QPoint left_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                         road_element->pos().y() + m_default_cell_size.height()/2);
                    QPoint left_pos_mapped_to_grid = map_point_to_grid_point(left_half_center_pos);

                    QPoint right_half_center_pos = QPoint(road_element->pos().x() + m_default_cell_size.width()/2,
                                                          road_element->pos().y() + 1.5*m_default_cell_size.height());
                    QPoint right_pos_mapped_to_grid = map_point_to_grid_point(right_half_center_pos);

                    if((std::find(m_path_to_destination.begin(), m_path_to_destination.end(), left_pos_mapped_to_grid) != m_path_to_destination.end())
                        || (std::find(m_path_to_destination.begin(), m_path_to_destination.end(), right_pos_mapped_to_grid) != m_path_to_destination.end())){
                        destination_direction = 3;
                        return destination_direction;
                    }
                }
            }
        }
    }

    return destination_direction;
}

double CCar::normal_speed_to_simulation_speed(int speed)
{
    return ((double)speed*10)/36;
}

void CCar::choose_direction_on_crossing_with_lights()
{
    m_was_leaving_a_crossing = true;
    QVector<int> allowed_directions = get_allowed_crossing_directions();

    if(allowed_directions.isEmpty()){
        if(m_has_designated_destination){
            destination_reached();
        }

        m_previous_movement_state = m_current_movement_state;
        m_current_movement_state = ECarMovementStates::turning_around;
    }

    int destination_direction = -1;

    if(m_has_designated_destination){
        destination_direction = find_crossing_side_to_destination(allowed_directions);
        if(destination_direction == -1){
            destination_reached(); //Destination lost in reality
        }
    }

    int direction_chosen = 0;
    if(destination_direction != -1){
        direction_chosen = destination_direction;
    }
    else{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, allowed_directions.size() - 1); //No turning around on crossings for now
        int direction_it = dist(gen);
        direction_chosen = allowed_directions[direction_it];
    }

    //It could be simplified, 90 degress of rotation <=> shift side's value by one
    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            if(direction_chosen == 1){
                m_current_movement_state = ECarMovementStates::riding_through_crossing;
            }
            else if(direction_chosen == 0){
                bool turning_left_possible = check_if_turning_left_on_crossing_with_lights_is_possible();
                if(turning_left_possible){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 2){
                m_turn_width = m_crossing->boundingRect().width();
                m_current_movement_state = ECarMovementStates::taking_right_turn;
            }
        }
        else{
            if(direction_chosen == 3){
                m_current_movement_state = ECarMovementStates::riding_through_crossing;
            }
            else if(direction_chosen == 2){
                bool turning_left_possible = check_if_turning_left_on_crossing_with_lights_is_possible();
                if(turning_left_possible){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 0){
                m_turn_width = m_crossing->boundingRect().width();
                m_current_movement_state = ECarMovementStates::taking_right_turn;
            }
        }
    }
    else{
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            if(direction_chosen == 0){
                m_current_movement_state = ECarMovementStates::riding_through_crossing;
            }
            else if(direction_chosen == 3){
                bool turning_left_possible = check_if_turning_left_on_crossing_with_lights_is_possible();
                if(turning_left_possible){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 1){
                m_turn_width = m_crossing->boundingRect().width();
                m_current_movement_state = ECarMovementStates::taking_right_turn;
            }
        }
        else{
            if(direction_chosen == 2){
                m_current_movement_state = ECarMovementStates::riding_through_crossing;
            }
            else if(direction_chosen == 1){
                bool turning_left_possible = check_if_turning_left_on_crossing_with_lights_is_possible();
                if(turning_left_possible){
                    m_turn_width = m_crossing->boundingRect().width();
                    m_current_movement_state = ECarMovementStates::taking_left_turn;
                }
                else{
                    return;
                }
            }
            else if(direction_chosen == 3){
                m_turn_width = m_crossing->boundingRect().width();
                m_current_movement_state = ECarMovementStates::taking_right_turn;
            }
        }
    }
}
QVector<int> CCar::get_allowed_crossing_directions()
{
    auto valid_sides = m_crossing->get_crossing_valid_sides();
    QVector<int> allowed_directions;

    int x_offset = 5;
    int y_offset = 5;

    QPointF possible_no_entry_sign_pos = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() + x_offset,
                                                 m_crossing->pos().y() - m_default_cell_size.width() + y_offset);

    auto items_at_sign_pos = m_map->items(possible_no_entry_sign_pos);
    for(auto item : items_at_sign_pos){
        auto prohibition_sign = dynamic_cast<CProhibitionSign*>(item);

        if(prohibition_sign && prohibition_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::no_entry){
            if(valid_sides.contains(0)){
                valid_sides.removeOne(0);
            }
        }
    }

    possible_no_entry_sign_pos = QPointF(m_crossing->pos().x() + m_crossing->boundingRect().width() + x_offset,
                                                 m_crossing->pos().y() + m_crossing->boundingRect().width() + y_offset);

    items_at_sign_pos = m_map->items(possible_no_entry_sign_pos);
    for(auto item : items_at_sign_pos){
        auto prohibition_sign = dynamic_cast<CProhibitionSign*>(item);

        if(prohibition_sign && prohibition_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::no_entry){
            if(valid_sides.contains(1)){
                valid_sides.removeOne(1);
            }
        }
    }

    possible_no_entry_sign_pos = QPointF(m_crossing->pos().x() - m_default_cell_size.width() + x_offset,
                                         m_crossing->pos().y() + m_crossing->boundingRect().width() + y_offset);

    items_at_sign_pos = m_map->items(possible_no_entry_sign_pos);
    for(auto item : items_at_sign_pos){
        auto prohibition_sign = dynamic_cast<CProhibitionSign*>(item);

        if(prohibition_sign && prohibition_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::no_entry){
            if(valid_sides.contains(2)){
                valid_sides.removeOne(2);
            }
        }
    }

    possible_no_entry_sign_pos = QPointF(m_crossing->pos().x() - m_default_cell_size.width() + x_offset,
                                         m_crossing->pos().y() - m_default_cell_size.width() + y_offset);

    items_at_sign_pos = m_map->items(possible_no_entry_sign_pos);
    for(auto item : items_at_sign_pos){
        auto prohibition_sign = dynamic_cast<CProhibitionSign*>(item);

        if(prohibition_sign && prohibition_sign->get_traffic_sign_type_as_int() == EProhibitionSigns::no_entry){
            if(valid_sides.contains(3)){
                valid_sides.removeOne(3);
            }
        }
    }

    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){

            //Sides from cars perspective
            if(m_allowed_horizontal_movements[0].second == true && valid_sides.contains(0)){ //left
                allowed_directions.append(0);
            }

            if(m_allowed_horizontal_movements[1].second == true && valid_sides.contains(2)){ //right
                allowed_directions.append(2);
            }

            if(m_allowed_vertical_movements[0].second == true && valid_sides.contains(1)){ //up (continue)
                allowed_directions.append(1);
            }
        }
        else{ // moving left
            if(m_allowed_horizontal_movements[0].second == true && valid_sides.contains(2)){ //left
                allowed_directions.append(2);
            }

            if(m_allowed_horizontal_movements[1].second == true && valid_sides.contains(0)){ //right
                allowed_directions.append(0);
            }

            if(m_allowed_vertical_movements[0].second == true && valid_sides.contains(3)){ //up (continue)
                allowed_directions.append(3);
            }
        }
    }
    else{ //Vertical
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){

            //Sides from cars perspective
            if(m_allowed_horizontal_movements[0].second == true && valid_sides.contains(3)){ //left
                allowed_directions.append(3);
            }

            if(m_allowed_horizontal_movements[1].second == true && valid_sides.contains(1)){ //right
                allowed_directions.append(1);
            }

            if(m_allowed_vertical_movements[0].second == true && valid_sides.contains(0)){ //up (continue)
                allowed_directions.append(0);
            }
        }
        else{ // moving down
            if(m_allowed_horizontal_movements[0].second == true && valid_sides.contains(1)){ //left
                allowed_directions.append(1);
            }

            if(m_allowed_horizontal_movements[1].second == true && valid_sides.contains(3)){ //right
                allowed_directions.append(3);
            }

            if(m_allowed_vertical_movements[0].second == true && valid_sides.contains(2)){ //up (continue)
                allowed_directions.append(2);
            }
        }
    }

    return allowed_directions;
}

bool CCar::check_if_turning_left_on_crossing_with_lights_is_possible()
{
    QGraphicsLineItem *collision_line;
    QPointF car_center_pos = this->pos();
    car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                             car_center_pos.y() + this->boundingRect().height()/2);

    if(this->get_current_movement_plane() == EMovementPlane::horizontal){
        if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
            collision_line = new QGraphicsLineItem(car_center_pos.x(), car_center_pos.y() - m_default_cell_size.width(),
                                                   car_center_pos.x() + 200, car_center_pos.y() - m_default_cell_size.width());
        }
        else{
            collision_line = new QGraphicsLineItem(car_center_pos.x(), car_center_pos.y() + m_default_cell_size.width(),
                                                   car_center_pos.x() - 200, car_center_pos.y() + m_default_cell_size.width());
        }
    }
    else{
        if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
            collision_line = new QGraphicsLineItem(car_center_pos.x() - m_default_cell_size.width(), car_center_pos.y(),
                                                   car_center_pos.x() - m_default_cell_size.width(), car_center_pos.y() - 200);
        }
        else{
            collision_line = new QGraphicsLineItem(car_center_pos.x() + m_default_cell_size.width(), car_center_pos.y(),
                                                   car_center_pos.x() + m_default_cell_size.width(), car_center_pos.y() + 200);
        }
    }

    m_map->addItem(collision_line);
    auto items_on_collision_course = m_map->collidingItems(collision_line);
    m_map->removeItem(collision_line);
    delete collision_line;

    //Check for cars on collision course
    for(auto item : items_on_collision_course){
        auto car = dynamic_cast<CCar*>(item);

        if(!car){
            continue;
        }

        if(car && car == this){
            continue;
        }

        if(car && !(car->is_waiting() || car->is_braking())){
            return false;
        }
    }

    return true;
}

bool CCar::check_if_driving_straight_through_crossing_without_lights_is_possible()
{

    if(m_current_road_priority_sign.first == ETrafficSignCategory::information_sign){
        if(m_current_road_priority_sign.second == EInformationSigns::priority_road){
            return true;
        }
    }
    else if(m_there_is_road_priority_sign_to_check){

        QGraphicsRectItem *collision_rect;
        QGraphicsRectItem *collision_on_turn_rect;

        if(this->get_current_movement_plane() == EMovementPlane::horizontal){
            collision_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 300);
            collision_rect->setPos(m_crossing->pos().x(), m_crossing->pos().y() - 150 + m_crossing->boundingRect().width()/2);

            if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() + m_crossing->boundingRect().width(),
                                               m_crossing->pos().y());
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50 , m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() - 50,
                                               m_crossing->pos().y());
            }
        }
        else{
            collision_rect = new QGraphicsRectItem(0, 0, 300, m_crossing->boundingRect().height());
            collision_rect->setPos(m_crossing->pos().x() - 150 + m_crossing->boundingRect().width()/2, m_crossing->pos().y());

            if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() - 50);
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() + m_crossing->boundingRect().height());
            }
        }

        m_map->addItem(collision_rect);
        auto colliding_items = m_map->collidingItems(collision_rect);
        m_map->removeItem(collision_rect);
        delete collision_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car && car != this){
                if(m_current_movement_plane == EMovementPlane::horizontal){
                    if(car->pos().y() > m_crossing->pos().y() && car->pos().y() < m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().y() < m_crossing->pos().y()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::down){
                            return false;
                        }
                    }
                    else if(car->pos().y() > m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::up){
                            return false;
                        }
                    }
                }
                else{
                    if(car->pos().x() > m_crossing->pos().x() && car->pos().x() < m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().x() < m_crossing->pos().x()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                            return false;
                        }
                    }
                    else if(car->pos().x() > m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::left){
                            return false;
                        }
                    }
                }
            }
        }

        m_map->addItem(collision_on_turn_rect);
        colliding_items = m_map->collidingItems(collision_on_turn_rect);
        m_map->removeItem(collision_on_turn_rect);
        delete collision_on_turn_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car == this){
                continue;
            }

            if(car && car->is_turning_left()){
                return false;
            }
        }
    }
    else{ //Equivalent intersection
        QGraphicsRectItem *collision_rect;

        if(this->get_current_movement_plane() == EMovementPlane::horizontal){
            collision_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 180);

            if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                collision_rect->setPos(m_crossing->pos());
            }
            else{
                collision_rect->setPos(m_crossing->pos().x(), m_crossing->pos().y() - 180 + m_crossing->boundingRect().height());
            }
        }
        else{
            collision_rect = new QGraphicsRectItem(0, 0, 180, m_crossing->boundingRect().height());

            if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
                collision_rect->setPos(m_crossing->pos());
            }
            else{
                collision_rect->setPos(m_crossing->pos().x() - 180 + m_crossing->boundingRect().width(), m_crossing->pos().y());
            }
        }

        m_map->addItem(collision_rect);
        auto colliding_items = m_map->collidingItems(collision_rect);
        m_map->removeItem(collision_rect);
        delete collision_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car && car != this){
                if(m_current_movement_plane == EMovementPlane::horizontal){
                    if(car->pos().y() > m_crossing->pos().y() && car->pos().y() < m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    } //Moving in the way of the direction
                    else if(car->pos().y() < m_crossing->pos().y()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::down){
                            return false;
                        }
                    }
                    else if(car->pos().y() > m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::up){
                            return false;
                        }
                    }
                }
                else{
                    if(car->pos().x() > m_crossing->pos().x() && car->pos().x() < m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().x() < m_crossing->pos().x()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                            return false;
                        }
                    }
                    else if(car->pos().x() > m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::left){
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool CCar::check_if_driving_left_through_crossing_without_lights_is_possible()
{
    if(m_current_road_priority_sign.first == ETrafficSignCategory::information_sign){
        if(m_current_road_priority_sign.second == EInformationSigns::priority_road){
            return true;
        }
    }
    else if(m_there_is_road_priority_sign_to_check){

        QGraphicsRectItem *collision_rect;
        QGraphicsRectItem *collision_on_turn_rect;

        if(this->get_current_movement_plane() == EMovementPlane::horizontal){
            collision_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 300);
            collision_rect->setPos(m_crossing->pos().x(), m_crossing->pos().y() - 150 + m_crossing->boundingRect().width()/2);

            if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() + m_crossing->boundingRect().width(),
                                               m_crossing->pos().y());
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() - 50,
                                               m_crossing->pos().y());
            }
        }
        else{
            collision_rect = new QGraphicsRectItem(0, 0, 300, m_crossing->boundingRect().height());
            collision_rect->setPos(m_crossing->pos().x() - 150 + m_crossing->boundingRect().width()/2, m_crossing->pos().y());

            if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() - 50);
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() + m_crossing->boundingRect().height());
            }
        }

        m_map->addItem(collision_rect);
        auto colliding_items = m_map->collidingItems(collision_rect);
        m_map->removeItem(collision_rect);
        delete collision_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car && car != this){
                if(m_current_movement_plane == EMovementPlane::horizontal){
                    if(car->pos().y() > m_crossing->pos().y() && car->pos().y() < m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().y() < m_crossing->pos().y()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::down){
                            return false;
                        }
                    }
                    else if(car->pos().y() > m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::up){
                            return false;
                        }
                    }
                }
                else{
                    if(car->pos().x() > m_crossing->pos().x() && car->pos().x() < m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().x() < m_crossing->pos().x()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                            return false;
                        }
                    }
                    else if(car->pos().x() > m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::left){
                            return false;
                        }
                    }
                }
            }
        }

        m_map->addItem(collision_on_turn_rect);
        colliding_items = m_map->collidingItems(collision_on_turn_rect);
        m_map->removeItem(collision_on_turn_rect);
        delete collision_on_turn_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car == this){
                continue;
            }

            if(car && car->is_turning_left()){
                return true;
            }
            else{
                return false;
            }
        }
    }
    else{ //Equivalent intersection
        QGraphicsRectItem *collision_rect;
        QGraphicsRectItem *collision_on_turn_rect;

        if(this->get_current_movement_plane() == EMovementPlane::horizontal){
            collision_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 180);

            if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() + m_crossing->boundingRect().width(),
                                               m_crossing->pos().y());
                collision_rect->setPos(m_crossing->pos());
            }
            else{
                collision_rect->setPos(m_crossing->pos().x(), m_crossing->pos().y() - 180 + m_crossing->boundingRect().height());
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() - 50,
                                               m_crossing->pos().y());
            }
        }
        else{
            collision_rect = new QGraphicsRectItem(0, 0, 180, m_crossing->boundingRect().height());

            if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
                collision_rect->setPos(m_crossing->pos());
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() - 50);
            }
            else{
                collision_rect->setPos(m_crossing->pos().x() - 180 + m_crossing->boundingRect().width(), m_crossing->pos().y());
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() + m_crossing->boundingRect().height());
            }
        }

        m_map->addItem(collision_rect);
        auto colliding_items = m_map->collidingItems(collision_rect);
        m_map->removeItem(collision_rect);
        delete collision_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car == this){
                continue;
            }

            if(car && car != this && !(car->is_turning_right())){
                if(m_current_movement_plane == EMovementPlane::horizontal){
                    if(car->pos().y() > m_crossing->pos().y() && car->pos().y() < m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().y() < m_crossing->pos().y()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::down){
                            return false;
                        }
                    }
                    else if(car->pos().y() > m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::up){
                            return false;
                        }
                    }
                }
                else{
                    if(car->pos().x() > m_crossing->pos().x() && car->pos().x() < m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().x() < m_crossing->pos().x()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                            return false;
                        }
                    }
                    else if(car->pos().x() > m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::left){
                            return false;
                        }
                    }
                }
            }
        }

        m_map->addItem(collision_on_turn_rect);
        colliding_items = m_map->collidingItems(collision_on_turn_rect);
        m_map->removeItem(collision_on_turn_rect);
        delete collision_on_turn_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car == this){
                continue;
            }

            if(car->is_turning_left()){
                return true;
            }
            else{
                return false;
            }
        }
    }

    return true;
}

bool CCar::check_if_driving_right_through_crossing_without_lights_is_possible()
{
    if(m_current_road_priority_sign.first == ETrafficSignCategory::information_sign){
        if(m_current_road_priority_sign.second == EInformationSigns::priority_road){
            return true;
        }
    }
    else if(m_there_is_road_priority_sign_to_check){

        QGraphicsRectItem *collision_rect;
        QGraphicsRectItem *collision_on_turn_rect;

        if(this->get_current_movement_plane() == EMovementPlane::horizontal){
            collision_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 300);
            collision_rect->setPos(m_crossing->pos().x(), m_crossing->pos().y() - 150 + m_crossing->boundingRect().width()/2);

            if(this->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() + m_crossing->boundingRect().width(),
                                               m_crossing->pos().y());
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, 50, m_crossing->boundingRect().width());
                collision_on_turn_rect->setPos(m_crossing->pos().x() - 50,
                                               m_crossing->pos().y());
            }
        }
        else{
            collision_rect = new QGraphicsRectItem(0, 0, 300, m_crossing->boundingRect().height());
            collision_rect->setPos(m_crossing->pos().x() - 150 + m_crossing->boundingRect().width()/2, m_crossing->pos().y());

            if(this->get_vertical_move_direction() == EVerticalMoveDirection::up){
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() - 50);
            }
            else{
                collision_on_turn_rect = new QGraphicsRectItem(0, 0, m_crossing->boundingRect().width(), 50);
                collision_on_turn_rect->setPos(m_crossing->pos().x(),
                                               m_crossing->pos().y() + m_crossing->boundingRect().height());
            }
        }

        m_map->addItem(collision_rect);
        auto colliding_items = m_map->collidingItems(collision_rect);
        m_map->removeItem(collision_rect);
        delete collision_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(car && car != this){
                if(m_current_movement_plane == EMovementPlane::horizontal){
                    if(car->pos().y() > m_crossing->pos().y() && car->pos().y() < m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().y() < m_crossing->pos().y()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::down){
                            return false;
                        }
                    }
                    else if(car->pos().y() > m_crossing->pos().y() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::vertical && car->get_vertical_move_direction() == EVerticalMoveDirection::up){
                            return false;
                        }
                    }
                }
                else{
                    if(car->pos().x() > m_crossing->pos().x() && car->pos().x() < m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        //On the crossing
                        return false;
                    }
                    else if(car->pos().x() < m_crossing->pos().x()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::right){
                            return false;
                        }
                    }
                    else if(car->pos().x() > m_crossing->pos().x() + m_crossing->boundingRect().width()){
                        if(car->get_current_movement_plane() == EMovementPlane::horizontal && car->get_horizontal_move_direction() == EHorizontalMoveDirection::left){
                            return false;
                        }
                    }
                }
            }
        }

        m_map->addItem(collision_on_turn_rect);
        colliding_items = m_map->collidingItems(collision_on_turn_rect);
        m_map->removeItem(collision_on_turn_rect);
        delete collision_on_turn_rect;

        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(car && car != this && car->is_turning_left()){
                return false;
            }
        }
    }
    else{
        auto colliding_items = m_map->collidingItems(m_crossing);
        for(auto item : colliding_items){
            auto car = dynamic_cast<CCar*>(item);

            if(!car){
                continue;
            }

            if(car == this){
                continue;
            }
            else{
                return false;
            }
        }
        return true;
    }

    return true;
}

bool CCar::check_if_turning_around_is_possible(CRoadway *roadway)
{
    QPointF car_center_pos = this->pos();
    car_center_pos = QPointF(car_center_pos.x() + this->boundingRect().width()/2,
                             car_center_pos.y() + this->boundingRect().height()/2);

    int roadside_offset = 5;
    int total_roadside_offset = 2*roadside_offset;

    int roadway_width = (roadway->get_carriageways_number()) * (roadway->get_lanes_number()) * m_default_cell_size.width();
    int roadway_offset = roadway_width - m_default_cell_size.width();
    int total_offset = roadway_offset - total_roadside_offset;

    QGraphicsLineItem *collision_line = nullptr;

    if(m_current_movement_plane == EMovementPlane::horizontal){
        if(m_horizontal_move_direction == EHorizontalMoveDirection::left){
            collision_line = new QGraphicsLineItem(car_center_pos.x() - 60, car_center_pos.y() + total_offset,
                                                   car_center_pos.x() + 30, car_center_pos.y() + total_offset);
        }
        else{
            collision_line = new QGraphicsLineItem(car_center_pos.x() - 30, car_center_pos.y() - total_offset,
                                                   car_center_pos.x() + 60, car_center_pos.y() - total_offset);
        }
    }
    else{
        if(m_vertical_move_direction == EVerticalMoveDirection::up){
            collision_line = new QGraphicsLineItem(car_center_pos.x() - total_offset, car_center_pos.y() - 60,
                                                   car_center_pos.x() - total_offset, car_center_pos.y() + 30);
        }
        else{
            collision_line = new QGraphicsLineItem(car_center_pos.x() + total_offset, car_center_pos.y() - 30,
                                                   car_center_pos.x() + total_offset, car_center_pos.y() + 60);
        }
    }

    m_map->addItem(collision_line);
    auto colliding_items = m_map->collidingItems(collision_line);
    m_map->removeItem(collision_line);
    delete collision_line;

    for(auto item : colliding_items){
        auto car = dynamic_cast<CCar*>(item);

        if(car && car != this){
            return false;
        }
    }

    return true;
}

CRoadUser *CCar::create_car()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);

    int random_number = dist(gen);

    QPixmap car_pixmap;

    switch (random_number){
    case 0:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_green.png");
        break;
    case 1:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_red.png");
        break;
    case 2:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_yellow.png");
        break;
    default:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_red.png");
    }

    return new CCar(car_pixmap, "Car", EMovementPlane::horizontal, EHorizontalMoveDirection::right);
}

void CCar::move(CReadOnlyMap *map)
{
    m_map = map;

    if(m_has_designated_destination){
        auto last_point = m_path_to_destination[m_path_to_destination.size() - 1];

        QPoint center_point = QPoint(this->pos().x() + this->boundingRect().width()/2,
                                     this->pos().y() + this->boundingRect().height()/2);
        QPoint current_grid_point = map_point_to_grid_point(center_point);

        //Do not turn around just to pass through the point at the other lane
        if((abs(current_grid_point.x() - last_point.x()) + abs(current_grid_point.y() - last_point.y())) < 2){
            destination_reached();
        }
    }

    auto it = m_movement_states_mapped_to_func.find(m_current_movement_state);
    if (it != m_movement_states_mapped_to_func.end()) {
        (this->*(it.value()))();
    }

    if(is_out_of_bounds(m_map)){
        this->hide();
    }
}

void CCar::reset_state()
{
    m_lights_present_but_inactive = false;
    m_was_waiting_for_simulation_to_start = false;
    m_rotation_remaining = 0;
    m_arc_length_remaining = 0;
    m_movement_planes_changed = 0;
    m_turn_started = 0;
    m_is_braking = false;
    m_is_waiting = false;
    m_crossing_after_p_crossing = false;
    m_was_leaving_a_crossing = false;
    m_is_turning_left = false;
    m_is_turning_right = false;
    m_there_is_road_priority_sign_to_check = false;

    //What different signs boil down to
    m_allowed_horizontal_movements =
        {{EHorizontalMoveDirection::left, true}, {EHorizontalMoveDirection::right, true}};

    m_allowed_vertical_movements =
        {{EVerticalMoveDirection::up, true}, {EVerticalMoveDirection::down, true}};

    m_turn_width = 0;
    m_crossing_side_chosen = 0;

    m_riding_through_crossing_offset = 100;
    m_global_speed_limit = (50*10)/36; //For now, city speed limit
    m_temporary_speed_limit = -1;
    m_current_speed = 0;
    update_speed_vector();

    m_current_movement_state = ECarMovementStates::riding;
    m_previous_movement_state = ECarMovementStates::riding;
}
