#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

#include "cexpandablewidget.h"
#include "ctrafficlightsdurationinputwidget.h"

CTrafficLightsDurationInputWidget::CTrafficLightsDurationInputWidget() {

    auto *content_widget = new QFrame(this);
    auto *widget_layout = new QVBoxLayout();
    auto *wrapper_layout = new QFormLayout();
    auto *expandable_widget = new CExpandableWidget("Traffic lights durations", 300, this);

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

    wrapper_layout->addRow("Green light duration [s]", &m_green_light_duration_spin_box);
    wrapper_layout->addRow("Yellow light duration [s]", &m_yellow_light_duration_spin_box);
    wrapper_layout->addRow("Red light duration [s]", &m_red_light_duration_spin_box);
    wrapper_layout->addRow("Red and yellow light duration [s]", &m_yellow_and_red_light_duration_spin_box);

    content_widget->setLayout(widget_layout);
    wrapper_layout->addWidget(content_widget);
    expandable_widget->set_content_layout(*wrapper_layout);

    expandable_widget->slot_toggle(true);
}

STrafficLightsDuration CTrafficLightsDurationInputWidget::get_traffic_lights_duration()
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
