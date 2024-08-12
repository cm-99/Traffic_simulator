#ifndef CSIMULATIONCONFIGURATIONDIALOG_H
#define CSIMULATIONCONFIGURATIONDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

#include "ctrafficlightsdurationinputwidget.h"
#include "croadusersparametersinputwidget.h"
#include "Logic/csimulationcontroller.h"

class CSimulationConfigurationDialog : public QDialog
{
public:
    CSimulationConfigurationDialog();
    SSimulationConfiguration get_simulation_configuration();

private:
    CRoadUsersParametersInputWidget m_cars_parameters_input_widget{"Cars simulation parameters"};
    CRoadUsersParametersInputWidget m_pedestrians_parameters_input_widget{"Pedestrians simulation parameters"};
    CTrafficLightsDurationInputWidget m_traffic_lights_duration_input_widget;
    SSimulationConfiguration m_simulation_configuration;
};

#endif // CSIMULATIONCONFIGURATIONDIALOG_H
