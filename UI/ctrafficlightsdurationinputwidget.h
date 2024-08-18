#ifndef CTRAFFICLIGHTSDURATIONINPUTWIDGET_H
#define CTRAFFICLIGHTSDURATIONINPUTWIDGET_H

#include <QFrame>
#include <QSpinBox>

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"

class CTrafficLightsDurationInputWidget : public QFrame
{
public:
    CTrafficLightsDurationInputWidget();
    CTrafficLightsDurationInputWidget(STrafficLightsDuration traffic_lights_duration);
    STrafficLightsDuration get_traffic_lights_duration();
    void check_input_validity();

private:
    QSpinBox m_green_light_duration_spin_box;
    QSpinBox m_yellow_light_duration_spin_box;
    QSpinBox m_yellow_and_red_light_duration_spin_box;
    QSpinBox m_red_light_duration_spin_box;
};

#endif // CTRAFFICLIGHTSDURATIONINPUTWIDGET_H
