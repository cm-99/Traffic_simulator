#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"

CTrafficLight::CTrafficLight(ETrafficLightsType traffic_ligths_type, QVector<QPixmap> lights_configurations_pixmaps,
                             STrafficLightsDuration traffic_lights_duration, bool is_conditional_turn_present) :
    CTrafficControlElement(ETrafficControlElementType::traffic_lights),
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

    setPixmap(m_traffic_lights_cycle[0].m_traffic_lights_pixmap);
    connect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_change_lights);
}

void CTrafficLight::set_is_active(bool is_active)
{
    if(is_active == m_is_active)
        return;

    if(is_active){ // Traffic lights should be activated - start from red
        m_is_active = is_active;
        m_current_phase_index = 1;
        m_current_lights_phase = m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_phase;
        setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);

        disconnect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_blink_yellow);
        m_lights_timer.start((m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_duration)/m_simulation_speed);
    }
    else{ // Traffic lights should be deactivated - blinking yellow
        m_is_active = is_active;
        m_lights_timer.stop();
        disconnect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_change_lights);

        m_current_phase_index = -2;
        setPixmap(m_inactive_lights_pixmap);
        connect(&m_lights_timer, &QTimer::timeout, this, &CTrafficLight::slot_blink_yellow);
        m_lights_timer.start(500/m_simulation_speed);
    }
}

void CTrafficLight::set_simulation_speed(int simulation_speed)
{
    if(m_current_phase_index == -1){ // Simulation hasn't started
        m_simulation_speed = simulation_speed;
        return;
    }

    int remaining_time_msc = m_lights_timer.remainingTime();
    if(remaining_time_msc == -1) // Simulation started but timer inactive - it is paused
        remaining_time_msc = m_lights_timer.interval();

    // Should work properly for both active and inactive/blinking
    int remaining_time_with_new_speed = (remaining_time_msc * m_simulation_speed)/simulation_speed;

    m_simulation_speed = simulation_speed;
    m_lights_timer.start(remaining_time_with_new_speed);
}

void CTrafficLight::synchronize_lights_with_offset()
{
    int phases_accumulated_time = 0;
    for(int i = 0; i < m_traffic_lights_cycle.size(); i++){
        phases_accumulated_time += m_traffic_lights_cycle[i].m_traffic_lights_duration;

        if(m_lights_simulation_offset_time <= phases_accumulated_time)
        {
            m_current_lights_phase = m_traffic_lights_cycle[i].m_traffic_lights_phase;
            int remaining_phase_time_msc = m_traffic_lights_cycle[i].m_traffic_lights_duration - phases_accumulated_time;

            m_current_phase_index = i;
            m_current_lights_phase = m_traffic_lights_cycle[i].m_traffic_lights_phase;
            setPixmap(m_traffic_lights_cycle[m_current_phase_index].m_traffic_lights_pixmap);
            m_lights_timer.start(remaining_phase_time_msc/m_simulation_speed);
        }
    }
}

void CTrafficLight::start_simulation()
{
    int full_lights_cycle_time = m_traffic_ligths_durations.full_lights_cycle_time();
    std::srand(std::time(nullptr));
    m_lights_simulation_offset_time = std::rand() % full_lights_cycle_time;
    synchronize_lights_with_offset();
}

void CTrafficLight::pause_simulation()
{
    int m_remaining_time_msc = m_lights_timer.remainingTime();
    m_lights_timer.stop();
    m_lights_timer.setInterval(m_remaining_time_msc);
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
        m_current_phase_index = 2;

        setPixmap(m_inactive_lights_pixmap);
        m_lights_timer.start(500/m_simulation_speed);
    }
}
