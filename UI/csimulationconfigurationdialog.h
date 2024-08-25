#ifndef CSIMULATIONCONFIGURATIONDIALOG_H
#define CSIMULATIONCONFIGURATIONDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

#include "ctrafficlightsdurationinputwidget.h"
#include "croadusersparametersrangeinputwidget.h"
#include "Logic/csimulationcontroller.h"

class CSimulationConfigurationDialog : public QDialog
{
public:
    CSimulationConfigurationDialog();
    static SSimulationConfiguration get_simulation_configuration();

private:
    CRoadUsersParametersRangeInputWidget *m_cars_parameters_input_widget;
    CRoadUsersParametersRangeInputWidget *m_pedestrians_parameters_input_widget;
    CTrafficLightsDurationInputWidget *m_traffic_lights_duration_input_widget;

    static SSimulationConfiguration m_simulation_configuration;
};

#endif // CSIMULATIONCONFIGURATIONDIALOG_H
