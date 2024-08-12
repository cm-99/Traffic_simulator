#ifndef CSIMULATIONCONTROLLER_H
#define CSIMULATIONCONTROLLER_H

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

    //TODO: recalculate everything with new config
    void set_simulation_configuration(SSimulationConfiguration simulation_configuration)
    {m_simulation_configuration = simulation_configuration;}

private:
    SSimulationConfiguration m_simulation_configuration;
};

#endif // CSIMULATIONCONTROLLER_H
