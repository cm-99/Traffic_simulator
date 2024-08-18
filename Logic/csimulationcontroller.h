#ifndef CSIMULATIONCONTROLLER_H
#define CSIMULATIONCONTROLLER_H

#include "Logic/cmapcreationcontroller.h"
#include "cbasecontroller.h"
#include "MapElements/croaduser.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"

struct SSimulationConfiguration{
    SRoadUsersBasicParameters m_cars_min_basic_parameters;
    SRoadUsersBasicParameters m_cars_mean_basic_parameters;
    SRoadUsersBasicParameters m_cars_max_basic_parameters;

    SRoadUsersBasicParameters m_pedestrians_min_basic_parameters;
    SRoadUsersBasicParameters m_pedestrians_mean_basic_parameters;
    SRoadUsersBasicParameters m_pedestrians_max_basic_parameters;

    STrafficLightsDuration m_traffic_lights_duration;

    bool m_place_road_users_automatically;
    int m_cars_to_place;
    int m_pedestrians_to_place;

    bool m_is_empty{true};

    SSimulationConfiguration() {};
    explicit SSimulationConfiguration(SRoadUsersBasicParameters cars_min_basic_parameters,
                                      SRoadUsersBasicParameters cars_mean_basic_parameters,
                                      SRoadUsersBasicParameters cars_max_basic_parameters,
                                      SRoadUsersBasicParameters pedestrians_min_basic_parameters,
                                      SRoadUsersBasicParameters pedestrians_mean_basic_parameters,
                                      SRoadUsersBasicParameters pedestrians_max_basic_parameters,
                                      STrafficLightsDuration traffic_lights_duration,
                                      bool place_road_users_automatically = false, int cars_to_place = 0, int pedestrians_to_place = 0){

        m_cars_min_basic_parameters = cars_min_basic_parameters;
        m_cars_mean_basic_parameters = cars_mean_basic_parameters;
        m_cars_max_basic_parameters = cars_max_basic_parameters;

        m_pedestrians_min_basic_parameters = pedestrians_min_basic_parameters;
        m_pedestrians_mean_basic_parameters = pedestrians_mean_basic_parameters;
        m_pedestrians_max_basic_parameters = pedestrians_max_basic_parameters;

        m_traffic_lights_duration = traffic_lights_duration;

        m_place_road_users_automatically = place_road_users_automatically;
        m_cars_to_place = cars_to_place;
        m_pedestrians_to_place = pedestrians_to_place;
        m_is_empty = false;
    }

    bool is_empty() {return m_is_empty;}
};

/**
 * @brief The CSimulationController class is a controller responsible for the traffic simulation process.
 *
 * The simulation is performed in discrete steps determined by a timer, the timeout of which is transmitted to the model
 * to update the state of the simulation.
 */

class CSimulationController : public CBaseController
{
public:
    CSimulationController (CApplicationController *application_controller, CEditableMap *map_model,
                          SSimulationConfiguration simulation_configuration);
    ~CSimulationController();

    void set_user_to_place_creation_func(CRoadUser *(*creation_func)(void));

    bool process_wheel_event(QWheelEvent *event);
    void process_mouse_move_event(QMouseEvent *event);
    void process_mouse_press_event(QMouseEvent *event);

    void start_simulation();
    void pause_simulation();
    void restart_simulation();

    bool simulation_is_ready() {return m_simulation_is_ready;}

    //TODO: recalculate everything with new config
    inline void set_simulation_configuration(SSimulationConfiguration simulation_configuration)
    {m_simulation_configuration = simulation_configuration;}
    inline void set_simulation_speed(uint simulation_speed) {m_simulation_speed = simulation_speed;}

private:
    SSimulationConfiguration m_simulation_configuration;
    QTimer m_simulation_timer;

    CRoadUser *m_road_user_being_placed{nullptr};
    CRoadUser *(*m_current_creation_func)(void);
    QGraphicsRectItem *m_validation_rect{nullptr};
    bool m_road_user_is_being_placed{false};

    void prepare_validation_rect();
    EMapElementPositionValidity get_current_user_position_validity();
    void update_validation_rect();
    int m_val_rect_size_offset{6};

    bool m_simulation_is_paused{true};
    bool m_simulation_is_ready{false};
    uint m_simulation_speed{1};

    void rotate_road_user_being_placed(QWheelEvent *event);
};

#endif // CSIMULATIONCONTROLLER_H
