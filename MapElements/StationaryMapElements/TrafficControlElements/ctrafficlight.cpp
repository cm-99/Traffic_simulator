#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"
#include <random>

CTrafficLight::CTrafficLight(ETrafficLightsType traffic_ligths_type, QVector<QPixmap> lights_configurations_pixmaps,
                             STrafficLightsDuration traffic_lights_duration, QString description, bool is_conditional_turn_present, QString pixmap_path) :
    CTrafficControlElement(ETrafficControlElementType::traffic_lights, description, pixmap_path),
    m_is_conditional_turn_present(is_conditional_turn_present),
    m_traffic_ligths_type(traffic_ligths_type),
    m_traffic_ligths_durations(traffic_lights_duration)
{
    if (lights_configurations_pixmaps.size() != 5){
        throw std::invalid_argument
            ("Invalid argument lights_phases_graphics_paths. Size is different that the total number of traffic light's configurations (5)");
    }

    m_inactive_lights_pixmap = lights_configurations_pixmaps[0];
    m_traffic_lights_cycle = {{ETrafficLightsPhase::red, traffic_lights_duration.m_red_ligth_duration, lights_configurations_pixmaps[1]},
                              {ETrafficLightsPhase::red_and_yellow, traffic_lights_duration.m_red_and_yellow_ligths_duration, lights_configurations_pixmaps[2]},
                              {ETrafficLightsPhase::green, traffic_lights_duration.m_green_ligth_duration, lights_configurations_pixmaps[3]},
                              {ETrafficLightsPhase::yellow, traffic_lights_duration.m_yellow_ligth_duration, lights_configurations_pixmaps[4]}};

    setPixmap(m_inactive_lights_pixmap);
    connect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_change_lights);
}

CStationaryMapElement *CTrafficLight::create_collision_possible_traffic_lights()
{
    ETrafficLightsType traffic_ligths_type = ETrafficLightsType::collision_possible;
    STrafficLightsDuration traffic_lights_durations = STrafficLightsDuration{58000, 3000, 58000, 1000};
    QVector<QPixmap> lights_configurations_pixmaps = {QPixmap(":graphics/map_elements_graphics/traffic_lights/collision_possible_traffic_lights_inactive.png"),
                                                      QPixmap(":graphics/map_elements_graphics/traffic_lights/collision_possible_traffic_lights_red.png"),
                                                      QPixmap(":graphics/map_elements_graphics/traffic_lights/collision_possible_traffic_lights_red_and_yellow.png"),
                                                      QPixmap(":graphics/map_elements_graphics/traffic_lights/collision_possible_traffic_lights_green.png"),
                                                      QPixmap(":graphics/map_elements_graphics/traffic_lights/collision_possible_traffic_lights_yellow.png")};
    QString description = "Collision possible";
    auto *collision_possible_traffic_lights = new CTrafficLight(traffic_ligths_type, lights_configurations_pixmaps,
                                                                traffic_lights_durations, description, false, "");
    return collision_possible_traffic_lights;
}

ETrafficLightsPhase CTrafficLight::get_pedestrians_lights_phase() const
{
    if(m_current_lights_phase == ETrafficLightsPhase::red){
        return ETrafficLightsPhase::green;
    }
    else{
        return ETrafficLightsPhase::red;
    }
}

QString CTrafficLight::serialize_as_string()
{
    return QString::number(pos().x()) + "," + QString::number(pos().y()) + "," + QString::number(m_traffic_ligths_type);
}

QString CTrafficLight::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::traffic_control_element) + "_" +
           QString::number(ETrafficControlElementType::traffic_lights);
}

CStationaryMapElement *CTrafficLight::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 3){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid traffic light serialization");
        return nullptr;
    }

    if(item_attributes_list[2].toInt() == ETrafficLightsType::collision_possible){
        CStationaryMapElement *item = create_collision_possible_traffic_lights();
        item->setPos(item_attributes_list[0].toInt(), item_attributes_list[1].toInt());
        return item;
    }
    else{
        //TODO: collision free lights
        return nullptr;
    }
}

void CTrafficLight::set_is_active(bool is_active, bool resynchronize)
{
    if(is_active == m_is_active)
        return;

    if(is_active){ // Traffic lights should be activated - IF simulation started - resynchronize if needed
        m_is_active = is_active;

        m_lights_timer.stop();
        disconnect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_blink_yellow);
        connect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_change_lights);

        if(m_is_linked){
            if(!m_corresponding_light->is_active()){
                m_corresponding_light->set_is_active(true, false);
            }
        }

        if(m_has_opposing_light){
            for(auto opposing_light : m_opposing_lights){
                if(!opposing_light->is_active()){
                    opposing_light->set_is_active(true, false);
                }
            }
        }

        if(m_simulation_has_started && resynchronize){
            set_ligths_durations(m_traffic_ligths_durations, true);
        }
    }
    else{ // Traffic lights should be deactivated - blinking yellow
        m_is_active = is_active;

        if(m_is_linked){
            if(m_corresponding_light->is_active()){
                m_corresponding_light->set_is_active(false);
            }
        }

        if(m_has_opposing_light){
            for(auto opposing_light : m_opposing_lights){
                if(opposing_light->is_active()){
                    opposing_light->set_is_active(false);
                }
            }
        }

        //m_lights_timer.setInterval(500/m_simulation_speed);
        disconnect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_change_lights);
        connect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_blink_yellow);

        if(m_simulation_has_started){
            m_lights_timer.stop();
            m_current_phase_index = -2;
            setPixmap(m_inactive_lights_pixmap);
            m_lights_timer.start(500/m_simulation_speed);
        }
    }
}

void CTrafficLight::set_ligths_durations(STrafficLightsDuration traffic_ligths_durations, bool manual_change, int additional_offset)
{
    if(traffic_ligths_durations.full_lights_cycle_time() < 1000){
        //Time in sec
        traffic_ligths_durations.m_red_ligth_duration = traffic_ligths_durations.m_red_ligth_duration*1000;
        traffic_ligths_durations.m_red_and_yellow_ligths_duration = traffic_ligths_durations.m_red_and_yellow_ligths_duration*1000;
        traffic_ligths_durations.m_green_ligth_duration = traffic_ligths_durations.m_green_ligth_duration*1000;
        traffic_ligths_durations.m_yellow_ligth_duration = traffic_ligths_durations.m_yellow_ligth_duration*1000;
    }

    m_traffic_ligths_durations = traffic_ligths_durations;

    m_traffic_lights_cycle[0].m_traffic_lights_duration = traffic_ligths_durations.m_red_ligth_duration;
    m_traffic_lights_cycle[1].m_traffic_lights_duration = traffic_ligths_durations.m_red_and_yellow_ligths_duration;
    m_traffic_lights_cycle[2].m_traffic_lights_duration = traffic_ligths_durations.m_green_ligth_duration;
    m_traffic_lights_cycle[3].m_traffic_lights_duration = traffic_ligths_durations.m_yellow_ligth_duration;

    if(manual_change && !m_automatic_synchronization_is_disabled){
        resynchronize_lights(additional_offset);
    }
}

void CTrafficLight::set_corresponding_light(CTrafficLight *corresponding_light)
{
    m_corresponding_light = corresponding_light;

    if(corresponding_light != nullptr){
        m_is_linked = true;
    }
}

void CTrafficLight::break_all_lights_linking()
{
    break_lights_link();
    break_opposing_lights_link();
    m_is_crossing_light = false;
}

void CTrafficLight::break_lights_link()
{
    if(m_is_linked){
        m_is_linked = false;
        if(m_corresponding_light->is_linked()){
            m_corresponding_light->break_lights_link();
        }
        m_corresponding_light = nullptr;
    }
}

void CTrafficLight::break_opposing_lights_link()
{
    if(m_has_opposing_light){
        m_has_opposing_light = false;
        for(auto opposing_light : m_opposing_lights){
            if(opposing_light->has_opposing_light()){
                opposing_light->break_opposing_lights_link();
            }
        }
    }
    m_opposing_lights.clear();
}

void CTrafficLight::add_opposing_light(CTrafficLight *opposing_light)
{
    m_has_opposing_light = true;
    if(!m_opposing_lights.contains(opposing_light)){
        m_opposing_lights.append(opposing_light);
    }
}

void CTrafficLight::set_lights_simulation_offset(int lights_simulation_offset)
{
    m_lights_simulation_offset_time = lights_simulation_offset;
    synchronize_lights_with_offset();
}

void CTrafficLight::set_simulation_speed(int simulation_speed)
{
    if(!m_simulation_has_started){
        m_simulation_speed = simulation_speed;
        return;
    }

    int remaining_time_msc = m_lights_timer.remainingTime();
    if(remaining_time_msc == -1){ // Simulation started but timer inactive - it is paused
        remaining_time_msc = (m_lights_timer.interval() * m_simulation_speed)/simulation_speed;
        m_lights_timer.setInterval(remaining_time_msc);
        return;
    }

    // Should work properly for both active and inactive/blinking
    int remaining_time_with_new_speed = (remaining_time_msc * m_simulation_speed)/simulation_speed;

    m_simulation_speed = simulation_speed;
    m_lights_timer.start(remaining_time_with_new_speed);
}

void CTrafficLight::synchronize_lights_with_offset()
{
    if(m_lights_simulation_offset_time > m_traffic_ligths_durations.full_lights_cycle_time()){
        m_lights_simulation_offset_time = m_lights_simulation_offset_time % m_traffic_ligths_durations.full_lights_cycle_time();
    }

    int phases_accumulated_time = 0;
    int remaining_phase_time_msc = 0;
    for(int i = 0; i < m_traffic_lights_cycle.size(); i++){

        phases_accumulated_time += m_traffic_lights_cycle[i].m_traffic_lights_duration;

        if(m_lights_simulation_offset_time <= phases_accumulated_time)
        {
            int previous_accumulated_time = phases_accumulated_time - m_traffic_lights_cycle[i].m_traffic_lights_duration;
            int remaining_offset_time = m_lights_simulation_offset_time - previous_accumulated_time;
            remaining_phase_time_msc = m_traffic_lights_cycle[i].m_traffic_lights_duration - remaining_offset_time;

            m_current_lights_phase = m_traffic_lights_cycle[i].m_traffic_lights_phase;
            m_current_phase_index = i;
            setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);
            break;
        }
    }
    m_is_synchronized = true;

    bool should_synchronize_other_lights = true;
    if(m_is_linked && m_corresponding_light->is_synchronized()){
        should_synchronize_other_lights = false;
    }

    if(m_has_opposing_light && should_synchronize_other_lights){
        for(auto opposing_light : m_opposing_lights){
            if(opposing_light->is_synchronized()){
                should_synchronize_other_lights = false;
                break;
            }
        }
    }

    if(!m_automatic_synchronization_is_disabled && should_synchronize_other_lights){
        if(m_is_linked){
            m_corresponding_light->set_lights_simulation_offset(m_lights_simulation_offset_time);
        }


        if(m_has_opposing_light){
            for(auto opposing_light : m_opposing_lights){
                opposing_light->set_lights_simulation_offset(m_lights_simulation_offset_time +
                                                             m_traffic_ligths_durations.full_lights_cycle_time() -
                                                             m_traffic_lights_cycle[0].m_traffic_lights_duration);
            }
        }
    }

    m_lights_timer.start(remaining_phase_time_msc/m_simulation_speed);
}

void CTrafficLight::resynchronize_lights(int additional_offset)
{
    if(additional_offset > m_traffic_ligths_durations.full_lights_cycle_time()){
        additional_offset = additional_offset % m_traffic_ligths_durations.full_lights_cycle_time();
    }

    int phases_accumulated_time = 0;
    int remaining_phase_time_msc = 0;
    for(int i = 0; i < m_traffic_lights_cycle.size(); i++){

        phases_accumulated_time += m_traffic_lights_cycle[i].m_traffic_lights_duration;

        if(additional_offset <= phases_accumulated_time)
        {
            int previous_accumulated_time = phases_accumulated_time - m_traffic_lights_cycle[i].m_traffic_lights_duration;
            int remaining_elapsed_time = additional_offset - previous_accumulated_time;
            remaining_phase_time_msc = m_traffic_lights_cycle[i].m_traffic_lights_duration - remaining_elapsed_time;

            m_current_lights_phase = m_traffic_lights_cycle[i].m_traffic_lights_phase;
            m_current_phase_index = i;
            setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);
            break;
        }
    }
    m_is_resynchronized = true;

    bool should_synchronize_other_lights = true;
    if(m_is_linked && m_corresponding_light->is_resynchronized()){
        should_synchronize_other_lights = false;
    }

    if(m_has_opposing_light && should_synchronize_other_lights){
        for(auto opposing_light : m_opposing_lights){
            if(opposing_light->is_resynchronized()){
                should_synchronize_other_lights = false;
                break;
            }
        }
    }

    if(!m_automatic_synchronization_is_disabled && should_synchronize_other_lights){
        if(m_is_linked){
            m_corresponding_light->set_ligths_durations(m_traffic_ligths_durations, true);
        }


        if(m_has_opposing_light){
            for(auto opposing_light : m_opposing_lights){
                opposing_light->set_ligths_durations(m_traffic_ligths_durations, true,
                                                     m_traffic_ligths_durations.full_lights_cycle_time() -
                                                         m_traffic_lights_cycle[0].m_traffic_lights_duration);
            }
        }
    }

    m_is_resynchronized = false;
    m_lights_timer.setInterval(remaining_phase_time_msc/m_simulation_speed);
}


void CTrafficLight::start_simulation()
{
    m_simulation_has_started = true;
    if(m_is_synchronized){
        return;
    }

    if(!m_is_active){
        m_lights_timer.start(500/m_simulation_speed);
        return;
    }

    if(!m_automatic_offset_is_disabled){

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, m_traffic_ligths_durations.full_lights_cycle_time());

        int random_number = dist(gen);
        m_lights_simulation_offset_time = random_number;
    }

    synchronize_lights_with_offset();
}

void CTrafficLight::pause_simulation()
{
    if(!m_simulation_has_started){
        return;
    }

    if(!m_is_active){
        m_lights_timer.setInterval(m_lights_timer.remainingTime());
        m_lights_timer.stop();
        return;
    }

    int m_remaining_time_msc = m_lights_timer.remainingTime();
    m_time_of_paused_phase_in_msc = m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_duration;
    m_lights_timer.stop();
    m_lights_timer.setInterval(m_remaining_time_msc);
}

void CTrafficLight::reset_to_default()
{
    m_simulation_has_started = false;
    m_is_synchronized = false;
    m_lights_simulation_offset_time = 0;
}

void CTrafficLight::slot_change_lights()
{
    if(m_current_phase_index + 1 == m_traffic_lights_cycle.size()) // Last phase reached - restart
        m_current_phase_index = 0;
    else
        m_current_phase_index = (m_current_phase_index + 1) % m_traffic_lights_cycle.size();

    m_current_lights_phase = m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_phase;
    setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);
    m_lights_timer.start((m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_duration)/m_simulation_speed);
}

void CTrafficLight::slot_blink_yellow()
{
    if(m_current_phase_index == -2){
        m_current_phase_index = 3;
        m_current_lights_phase = ETrafficLightsPhase::yellow;

        setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);
        m_lights_timer.start(500/m_simulation_speed);
    }
    else{
        m_current_phase_index = -2;

        setPixmap(m_inactive_lights_pixmap);
        m_lights_timer.start(500/m_simulation_speed);
    }
}
