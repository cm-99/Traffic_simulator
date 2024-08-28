#ifndef CSIMULATIONCONTROLLER_H
#define CSIMULATIONCONTROLLER_H

#include <random>

#include "Logic/cmapcreationcontroller.h"
#include "cbasecontroller.h"
#include "MapElements/croaduser.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"
#include "Logic/astarmappathfinder.h"

struct SSimulationConfiguration{
    SRoadUsersBasicParameters m_cars_min_basic_parameters;
    SRoadUsersBasicParameters m_cars_mean_basic_parameters;
    SRoadUsersBasicParameters m_cars_max_basic_parameters;

    SRoadUsersBasicParameters m_pedestrians_min_basic_parameters;
    SRoadUsersBasicParameters m_pedestrians_mean_basic_parameters;
    SRoadUsersBasicParameters m_pedestrians_max_basic_parameters;

    STrafficLightsDuration m_traffic_lights_duration;
    bool m_lights_automatic_synchronization_disabled;
    bool m_lights_random_offset_disabled;

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
                                      bool lights_automatic_synchronization_disabled,
                                      bool lights_random_offset_disabled,
                                      bool place_road_users_automatically = false, int cars_to_place = 0, int pedestrians_to_place = 0){

        m_cars_min_basic_parameters = cars_min_basic_parameters;
        m_cars_mean_basic_parameters = cars_mean_basic_parameters;
        m_cars_max_basic_parameters = cars_max_basic_parameters;

        m_pedestrians_min_basic_parameters = pedestrians_min_basic_parameters;
        m_pedestrians_mean_basic_parameters = pedestrians_mean_basic_parameters;
        m_pedestrians_max_basic_parameters = pedestrians_max_basic_parameters;

        m_traffic_lights_duration = traffic_lights_duration;
        m_lights_automatic_synchronization_disabled = lights_automatic_synchronization_disabled;
        m_lights_random_offset_disabled = lights_random_offset_disabled;

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
    Q_OBJECT
public:
    CSimulationController (CApplicationController *application_controller, CEditableMap *map_model,
                          SSimulationConfiguration simulation_configuration);
    ~CSimulationController();

    void set_user_to_place_creation_func(CRoadUser *(*creation_func)(void));

    /**
     * @brief Processes wheel event. If road user is being placed and it is rotable, it will be rotated according to the wheel event.
     * Otherwise relays the event to corresponding view.
     * @param event
     * @return returns true if event was handled; otherwise returns false
     */
    bool process_wheel_event(QWheelEvent *event);
    /**
     * @brief Processes mouse move event. If the road user is being placed, it follows the cursor.
     * @param event
     */
    void process_mouse_move_event(QMouseEvent *event);
    /**
     * @brief Processes mouse press event. Depending on the button pressed, modifiers and simulation state:
     * - (road user is being placed + left button) - places the road user (if position is valid)
     * - (road user is NOT being placed + simulation is paused + left button) - picks up the road user at mouse position
     * - (road user is being placed + right button) - removes the road user being placed
     *
     * - (simulation is paused + ctrl modifier + left button) - opens configuration dialog for road user at mouse position
     * - (road user is not being placed + shift modifier + left button) - allows destination change for road user at mouse position
     * - (simulation is NOT paused + left button) - opens configuration dialog for road user at mouse position
     * @param event
     */
    void process_mouse_press_event(QMouseEvent *event);

    void start_simulation();
    void pause_simulation();
    void resume_simulation();
    /**
     * @brief Restars simulation. If automatic road user's placement is enabled, they will be randomly placed again.
     * Road user's parameters are generated anew; traffic light's offsets are generated anew.
     */
    void restart_simulation();

    bool simulation_is_ready() {return m_simulation_is_ready;}

    /**
     * @brief Restarts simulation with new configuration.
     * @param simulation_configuration
     */
    void set_simulation_configuration(SSimulationConfiguration simulation_configuration);
    void set_simulation_speed(uint simulation_speed);
    /**
     * @brief Configures traffic lights which was previously selected by the user using provided parameters.
     * @param traffic_lights_duration
     * @param offset
     * @param is_disabled
     */
    void configure_traffic_light(STrafficLightsDuration traffic_lights_duration, int offset, bool is_disabled);
    /**
     * @brief Configures road user previously selected by the user with provided parameters.
     * @param parameters
     */
    void configure_road_user(SRoadUsersBasicParameters parameters);

private:
    //Simulation configuration attributes
    SSimulationConfiguration m_simulation_configuration;
    QTimer m_simulation_timer;
    int m_simulation_step_interval{100};
    bool m_simulation_was_started{false};
    bool m_simulation_is_paused{true};
    bool m_simulation_is_ready{false};
    uint m_simulation_speed{1};
    std::mt19937 *m_generator;
    QVector<std::piecewise_linear_distribution<>> m_pedestrian_attributes_distributions;
    QVector<std::piecewise_linear_distribution<>> m_cars_attributes_distributions;
    CTrafficLight *m_traffic_light_being_configured{nullptr};
    CRoadUser *m_road_user_being_configured{nullptr};
    QVector<QVector<EPermittedRoadUsers>> m_map_mobility_representation;

    //Simulation configuration methods
    std::piecewise_linear_distribution<> create_road_users_attribute_distribution(double min, double mean, double max);
    void prepare_pedestrians_attributes_distributions();
    void prepare_cars_attributes_distributions();
    SRoadUsersBasicParameters prepare_road_users_parameters(QVector<std::piecewise_linear_distribution<>> &attributes_distributions);
    void place_road_users_automatically();
    bool place_cars_automatically();
    bool place_pedestrians_automatically();
    void configure_road_user(CRoadUser *road_user);
    void prepare_map_mobility_grid();

    //Road users placement attributes
    CRoadUser *m_road_user_being_placed{nullptr};
    CRoadUser *(*m_current_creation_func)(void);
    QGraphicsRectItem *m_validation_rect{nullptr};
    bool m_road_user_is_being_placed{false};
    bool m_road_user_is_being_edited{false};

    //Road users placement methods
    void prepare_validation_rect();
    EMapElementPositionValidity get_road_user_position_validity(QPoint position, CRoadUser *road_user, bool map_to_scene = true);
    void update_validation_rect(QMouseEvent *event);
    int m_val_rect_size_offset{6};
    void process_mouse_press_event_in_paused_simulation(QMouseEvent *event);
    void process_mouse_press_event_in_running_simulation(QMouseEvent *event);
    void rotate_road_user_being_placed(QWheelEvent *event);
    void rotate_road_user(int rotation_change, CRoadUser *road_user);
    void place_user_and_prepare_next(QMouseEvent *event);
    QPoint get_road_user_legal_position(QPoint position, CRoadUser *road_user, bool map_to_scene = true);
    void erase_selected_user(QMouseEvent *event);
    int cars_roadside_offset{5};

    //Road users destination selection attributes
    QList<CRoadUser*> m_road_users_selected;
    QMap<CRoadUser*, QGraphicsEllipseItem*> m_road_users_mapped_to_selection_ellipses;
    ERoadUsers m_current_roads_users_being_selected;
    QGraphicsEllipseItem *m_destination_validation_item{nullptr};
    QMap<CRoadUser*, QGraphicsEllipseItem*> m_road_users_mapped_to_current_destination_items;
    AStarMapPathFinder m_path_finder;

    //Road users destination selection methods
    void add_road_user_with_destination_being_chosen(CRoadUser *road_user);
    void process_road_users_destination_selection(QPointF position);
    EMapElementPositionValidity validate_destination(QPointF position);
    void clear_users_destination_selection();
    QPoint map_position_to_mobility_grid(QPoint position);

private slots:
    void slot_process_simulation_step();

signals:
    void signal_road_user_configuration_requested(SRoadUsersBasicParameters parameters, ERoadUsers type);
    void signal_traffic_light_configuration_requested(STrafficLightsDuration traffic_lights_duration, bool automatic_offset_is_disabled, bool is_disabled);

};

#endif // CSIMULATIONCONTROLLER_H
