#ifndef CSIMULATIONPAGE_H
#define CSIMULATIONPAGE_H

#include <QPushButton>

#include "iuipage.h"
#include "Logic/csimulationcontroller.h"
#include "UI/croadusersplacementwidget.h"

class CSimulationPage : public IUIPage
{
public:
    CSimulationPage(CSimulationController *simulation_controller, QWidget *parent);
    ~CSimulationPage();

    void restore_to_default() override;
    void relay_creator_button_clicked(CRoadUser *(*creation_func)(void));

private:
    CSimulationController *m_simulation_controller;

    QVector<QIcon*> m_speed_button_icons;
    QPushButton *m_start_button;
    QPushButton *m_simulation_speed_button;
    QPushButton *m_restart_simulation_button;
    QPushButton *m_reconfigure_simulation_button;

    uint m_simulation_speed{1};
    bool m_simulation_is_paused{false};
    bool m_simulation_was_started{false};

private slots:
    void slot_change_simulation_speed();
    void slot_change_simulation_state();
    void slot_restart_simulation();
    void slot_reconfigure_simulation();
    void slot_request_traffic_light_configuration(STrafficLightsDuration traffic_lights_duration, bool automatic_offset_is_disabled, bool is_disabled);
    void slot_request_road_users_parameters(SRoadUsersBasicParameters parameters, ERoadUsers type);
};

#endif // CSIMULATIONPAGE_H
