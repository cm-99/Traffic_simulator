#ifndef CTRAFFICLIGHT_H
#define CTRAFFICLIGHT_H

#include <QTimer>
#include <QObject>

#include "MapElements/StationaryMapElements/ctrafficcontrolelement.h"

/**
 * @brief The STrafficLightsDuration struct stores information regarding the duration of each traffic lights configuration in seconds.
 */
struct STrafficLightsDuration{
    int m_green_ligth_duration;
    int m_yellow_ligth_duration;
    int m_red_ligth_duration;
    int m_red_and_yellow_ligths_duration;
    bool m_is_empty{true};

    explicit STrafficLightsDuration() {};
    STrafficLightsDuration(int green_ligth_duration, int yellow_ligth_duration, int red_ligth_duration, int red_and_yellow_ligths_duration):
        m_green_ligth_duration(green_ligth_duration),
        m_yellow_ligth_duration(yellow_ligth_duration),
        m_red_ligth_duration(red_ligth_duration),
        m_red_and_yellow_ligths_duration(red_and_yellow_ligths_duration),
        m_is_empty(false){};

    int full_lights_cycle_time(){
        return m_green_ligth_duration + m_yellow_ligth_duration + m_red_ligth_duration + m_red_and_yellow_ligths_duration;
    }

    bool is_empty() {return m_is_empty;}
};

/**
 * @brief The ETrafficLightsType enum is used to store information about the basic type of the traffic lights
 * collision_possible   - S1
 * collision_free       - S3
 */
enum ETrafficLightsType {collision_possible = 0, collision_free = 1};
/**
 * @brief The ETrafficLightsPhase is used to store information about traffic lights phase.
 */
enum class ETrafficLightsPhase {red, red_and_yellow, green, yellow};
/**
 * @brief The STrafficLightsCycle struct stores information about traffic lights cycle -
 * traffic lights phase, its duration and a pixmap graphically representing traffic lights during corresponding phase
 */
struct STrafficLightsCycle{
    ETrafficLightsPhase m_traffic_lights_phase;
    int m_traffic_lights_duration;
    QPixmap m_traffic_lights_pixmap;

    STrafficLightsCycle(ETrafficLightsPhase traffic_lights_phase, int traffic_lights_duration, QPixmap traffic_lights_pixmap) :
        m_traffic_lights_phase(traffic_lights_phase),
        m_traffic_lights_duration(traffic_lights_duration),
        m_traffic_lights_pixmap(traffic_lights_pixmap){};
};

/**
 * @brief The CTrafficLigth class is a graphical representation of traffic ligths.
 * It has the logic necessary to simulate the operation of lights.
 *
 * The lights in question are lights for drivers. The starting phase of lights is red.
 * It is assumed that a green light for pedestrians is lit when a red light is lit for drivers (IF there is a pedestrian crossing).
 *
 * POSSIBLE future options - give the user means to synchronize/desynchronize lights globally
 * TODO: traffic lights have predetermined cycles. Try to use some kind of list or phase-functor mapping to change lights.
 */
class CTrafficLight : public QObject, public CTrafficControlElement
{
    Q_OBJECT
public:

    static CStationaryMapElement *create_collision_possible_traffic_lights();
    const inline STrafficLightsDuration& get_traffic_lights_durations() const {return m_traffic_ligths_durations;}
    inline int get_lights_simulation_offset_time() const {return m_lights_simulation_offset_time;}
    inline ETrafficLightsPhase get_current_lights_phase() const {return m_current_lights_phase;}
    ETrafficLightsPhase get_pedestrians_lights_phase() const;
    inline bool is_active() const {return m_is_active;}
    inline bool is_linked() const {return m_is_linked;}
    inline bool is_synchronized() const {return m_is_synchronized;}
    inline bool is_resynchronized() const {return m_is_resynchronized;}
    inline bool has_opposing_light() const {return m_has_opposing_light;}

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

    void set_is_active(bool is_active, bool resynchronize = true);
    void set_ligths_durations(STrafficLightsDuration traffic_ligths_durations, bool manual_change = false, int additional_offset = 0);
    void set_corresponding_light(CTrafficLight *corresponding_light);
    void set_is_crossing_light() {m_is_crossing_light = true;}
    inline void desynchronize_light() {m_is_synchronized = false;}
    inline void is_automatic_synchronization_disabled(bool automatic_synchronization_is_disabled)
    {m_automatic_synchronization_is_disabled = automatic_synchronization_is_disabled;}
    inline void is_automatic_offset_disabled(bool automatic_offset_is_disabled) {m_automatic_offset_is_disabled = automatic_offset_is_disabled;}

    void break_all_lights_linking();
    void break_lights_link();
    void break_opposing_lights_link();

    void add_opposing_light(CTrafficLight *opposing_light);
    /**
     * @brief set_lights_simulation_offset is a method used to synchronize lights.
     * It's only important when starting or restarting simulation, mainly for the purpose of synchronizing lights on one crossing.
     * @param lights_simulation_offset - time in miliseconds to be used as lights simulation offset time.
     */
    void set_lights_simulation_offset(int lights_simulation_offset);
    void set_simulation_speed(int simulation_speed);
    void start_simulation();
    void pause_simulation();
    void resume_simulation() {m_lights_timer.start();}
    void reset_to_default();

private:
    /**
     * @brief CTrafficLight
     * @param traffic_ligths_type
     * @param lights_phases_pixmaps - pixmaps of traffic lights during corresponding configurations IN ORDER:
     * inactive
     * red
     * red_and_yellow
     * green
     * yellow
     * @param traffic_lights_duration
     * @param is_conditional_turn_present
     */
    CTrafficLight(ETrafficLightsType traffic_ligths_type, QVector<QPixmap> lights_configurations_pixmaps,
                  STrafficLightsDuration traffic_lights_duration, QString description, bool is_conditional_turn_present = false, QString pixmap_path = "");

    bool m_is_conditional_turn_present;
    ETrafficLightsType m_traffic_ligths_type;
    QVector<STrafficLightsCycle> m_traffic_lights_cycle;
    QPixmap m_inactive_lights_pixmap;

    int m_time_of_paused_phase_in_msc{0};
    int m_simulation_speed{1};
    QTimer m_lights_timer;
    int m_lights_simulation_offset_time{0};
    int m_current_phase_index{-1}; // -1 means that the simulation hasn't started; -2 means that traffic lights are disabled

    bool m_simulation_has_started{false};
    bool m_simulation_is_paused{false};
    bool m_is_active{true};
    ETrafficLightsPhase m_current_lights_phase;
    STrafficLightsDuration m_traffic_ligths_durations;
    bool m_is_linked{false};
    bool m_has_opposing_light{false};
    bool m_is_crossing_light{false};
    //Synchronized with other lights on the crossing
    bool m_is_synchronized{false};
    bool m_is_resynchronized{false};
    bool m_automatic_synchronization_is_disabled{false};
    bool m_automatic_offset_is_disabled{false};

    CTrafficLight *m_corresponding_light{nullptr};
    QList<CTrafficLight*> m_opposing_lights;

    void synchronize_lights_with_offset();

    void resynchronize_lights(int additional_offset);
public slots:
    void slot_change_lights();
    void slot_blink_yellow();
};

#endif // CTRAFFICLIGHT_H
