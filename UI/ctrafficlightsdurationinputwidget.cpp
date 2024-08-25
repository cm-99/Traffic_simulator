#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>

#include "ctrafficlightsdurationinputwidget.h"

CTrafficLightsDurationInputWidget::CTrafficLightsDurationInputWidget() {

    auto main_layout = new QFormLayout();
    main_layout->addWidget(new QLabel("Traffic lights durations"));

    //TODO: refactor this. Implement methods similar to SRoadUsersBasicParameters and use them instead
    m_green_light_duration_spin_box.setMinimum(10);
    m_green_light_duration_spin_box.setMaximum(70);
    m_green_light_duration_spin_box.setValue(58);

    m_yellow_light_duration_spin_box.setMinimum(1);
    m_yellow_light_duration_spin_box.setMaximum(10);
    m_yellow_light_duration_spin_box.setValue(3);

    m_red_light_duration_spin_box.setMinimum(10);
    m_red_light_duration_spin_box.setMaximum(70);
    m_red_light_duration_spin_box.setValue(58);

    m_yellow_and_red_light_duration_spin_box.setMinimum(1);
    m_yellow_and_red_light_duration_spin_box.setMaximum(5);
    m_yellow_and_red_light_duration_spin_box.setValue(1);

    main_layout->addRow("Green light duration [s]", &m_green_light_duration_spin_box);
    main_layout->addRow("Yellow light duration [s]", &m_yellow_light_duration_spin_box);
    main_layout->addRow("Red light duration [s]", &m_red_light_duration_spin_box);
    main_layout->addRow("Red and yellow light duration [s]", &m_yellow_and_red_light_duration_spin_box);

    this->setLayout(main_layout);
}

CTrafficLightsDurationInputWidget::CTrafficLightsDurationInputWidget(STrafficLightsDuration traffic_lights_duration)
{
    auto main_layout = new QFormLayout();
    main_layout->addWidget(new QLabel("Traffic lights durations"));

    m_green_light_duration_spin_box.setMinimum(10);
    m_green_light_duration_spin_box.setMaximum(70);
    m_green_light_duration_spin_box.setValue(traffic_lights_duration.m_green_ligth_duration);

    m_yellow_light_duration_spin_box.setMinimum(1);
    m_yellow_light_duration_spin_box.setMaximum(10);
    m_yellow_light_duration_spin_box.setValue(traffic_lights_duration.m_yellow_ligth_duration);

    m_red_light_duration_spin_box.setMinimum(10);
    m_red_light_duration_spin_box.setMaximum(70);
    m_red_light_duration_spin_box.setValue(traffic_lights_duration.m_red_ligth_duration);

    m_yellow_and_red_light_duration_spin_box.setMinimum(1);
    m_yellow_and_red_light_duration_spin_box.setMaximum(5);
    m_yellow_and_red_light_duration_spin_box.setValue(traffic_lights_duration.m_red_and_yellow_ligths_duration);

    main_layout->addRow("Green light duration [s]", &m_green_light_duration_spin_box);
    main_layout->addRow("Yellow light duration [s]", &m_yellow_light_duration_spin_box);
    main_layout->addRow("Red light duration [s]", &m_red_light_duration_spin_box);
    main_layout->addRow("Red and yellow light duration [s]", &m_yellow_and_red_light_duration_spin_box);

    this->setLayout(main_layout);
}

STrafficLightsDuration CTrafficLightsDurationInputWidget::get_traffic_lights_duration_in_sec()
{
    return STrafficLightsDuration(m_green_light_duration_spin_box.value(), m_yellow_light_duration_spin_box.value(),
                           m_red_light_duration_spin_box.value(), m_yellow_and_red_light_duration_spin_box.value());
}

void CTrafficLightsDurationInputWidget::check_input_validity()
{
    if(STrafficLightsDuration(m_green_light_duration_spin_box.value(), m_yellow_light_duration_spin_box.value(),
                               m_red_light_duration_spin_box.value(), m_yellow_and_red_light_duration_spin_box.value()).full_lights_cycle_time()
        > 120){
        QMessageBox::warning(this, "", "Full lights cycle duration should not be longer than 120 seconds in accordance with the guidelines"
                                       " of the road administration.");
    }
}
