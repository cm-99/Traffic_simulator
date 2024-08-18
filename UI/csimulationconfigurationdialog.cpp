#include <QPushButton>
#include <QStackedLayout>
#include <QFormLayout>
#include <QCheckBox>

#include "csimulationconfigurationdialog.h"

SSimulationConfiguration CSimulationConfigurationDialog::m_simulation_configuration = SSimulationConfiguration();

CSimulationConfigurationDialog::CSimulationConfigurationDialog()
{
    setResult(QDialog::DialogCode::Rejected);

    auto stacked_layout = new QStackedLayout();
    this->setLayout(stacked_layout);

    auto first_page_widget = new QWidget();
    auto first_page_layout = new QVBoxLayout();
    auto fp_next_button = new QPushButton("Next");

    auto second_page_widget = new QWidget();
    auto second_page_layout = new QVBoxLayout();
    auto sp_buttons_layout = new QHBoxLayout();
    auto sp_next_button = new QPushButton("Next");
    auto sp_previous_button = new QPushButton("Previous");

    auto third_page_widget = new QWidget();
    auto third_page_layout = new QVBoxLayout();
    auto tp_form_layout = new QFormLayout();
    auto automatic_placement_check_box = new QCheckBox();
    auto cars_number_spin_box = new QSpinBox();
    cars_number_spin_box->setDisabled(true);
    auto pedestrians_number_spin_box = new QSpinBox();
    pedestrians_number_spin_box->setDisabled(true);
    auto tp_buttons_layout = new QHBoxLayout();
    auto tp_previous_button = new QPushButton("Previous");
    auto start_simulation_button = new QPushButton("Start simulation");

    if(m_simulation_configuration.is_empty()){
        m_cars_parameters_input_widget = new CRoadUsersParametersInputWidget("Cars simulation parameters", ERoadUsers::car);
        m_pedestrians_parameters_input_widget = new CRoadUsersParametersInputWidget("Pedestrians simulation parameters",
                                                                                    ERoadUsers::pedestrian);
        m_traffic_lights_duration_input_widget = new CTrafficLightsDurationInputWidget();
    }
    else{
        m_cars_parameters_input_widget = new CRoadUsersParametersInputWidget("Cars simulation parameters",
        std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
        (m_simulation_configuration.m_cars_min_basic_parameters, m_simulation_configuration.m_cars_max_basic_parameters,
         m_simulation_configuration.m_cars_mean_basic_parameters));

        m_pedestrians_parameters_input_widget = new CRoadUsersParametersInputWidget("Pedestrians simulation parameters",
        std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
        (m_simulation_configuration.m_pedestrians_min_basic_parameters, m_simulation_configuration.m_pedestrians_max_basic_parameters,
         m_simulation_configuration.m_pedestrians_mean_basic_parameters));
        m_traffic_lights_duration_input_widget = new CTrafficLightsDurationInputWidget(m_simulation_configuration.m_traffic_lights_duration);
    }

    first_page_layout->addWidget(m_cars_parameters_input_widget);
    fp_next_button->setFixedSize(fp_next_button->sizeHint().width(), fp_next_button->sizeHint().height());
    first_page_layout->addWidget(fp_next_button);
    first_page_layout->setAlignment(fp_next_button, Qt::AlignRight);
    first_page_widget->setLayout(first_page_layout);

    second_page_layout->addWidget(m_pedestrians_parameters_input_widget);
    second_page_layout->addLayout(sp_buttons_layout);
    sp_next_button->setFixedSize(sp_next_button->sizeHint().width(), sp_next_button->sizeHint().height());
    sp_previous_button->setFixedSize(sp_previous_button->sizeHint().width(), sp_previous_button->sizeHint().height());
    sp_buttons_layout->addWidget(sp_previous_button);
    sp_buttons_layout->addWidget(sp_next_button);
    sp_buttons_layout->setAlignment(sp_previous_button, Qt::AlignLeft);
    sp_buttons_layout->setAlignment(sp_next_button, Qt::AlignRight);
    second_page_widget->setLayout(second_page_layout);

    third_page_layout->addWidget(m_traffic_lights_duration_input_widget);
    third_page_layout->addLayout(tp_form_layout);
    third_page_layout->addLayout(tp_buttons_layout);
    tp_form_layout->addRow("Automatically place cars and pedestrians", automatic_placement_check_box);
    tp_form_layout->addRow("Cars number", cars_number_spin_box);
    tp_form_layout->addRow("Pedestrians number", pedestrians_number_spin_box);
    tp_previous_button->setFixedSize(tp_previous_button->sizeHint().width(), tp_previous_button->sizeHint().height());
    start_simulation_button->setFixedSize(start_simulation_button->sizeHint().width(), start_simulation_button->sizeHint().height());
    tp_buttons_layout->addWidget(tp_previous_button);
    tp_buttons_layout->addWidget(start_simulation_button);
    tp_buttons_layout->setAlignment(tp_previous_button, Qt::AlignLeft);
    tp_buttons_layout->setAlignment(start_simulation_button, Qt::AlignRight);
    third_page_widget->setLayout(third_page_layout);

    stacked_layout->addWidget(first_page_widget);
    stacked_layout->addWidget(second_page_widget);
    stacked_layout->addWidget(third_page_widget);

    connect(fp_next_button, &QPushButton::clicked, this, [=](){
        stacked_layout->setCurrentIndex(1);
    });

    connect(sp_previous_button, &QPushButton::clicked, this, [=](){
        stacked_layout->setCurrentIndex(0);
    });

    connect(sp_next_button, &QPushButton::clicked, this, [=](){
        stacked_layout->setCurrentIndex(2);
    });

    connect(tp_previous_button, &QPushButton::clicked, this, [=](){
        stacked_layout->setCurrentIndex(1);
    });

    connect(automatic_placement_check_box, &QCheckBox::checkStateChanged, this, [=](){
        cars_number_spin_box->setDisabled(!automatic_placement_check_box->isChecked());
        pedestrians_number_spin_box->setDisabled(!automatic_placement_check_box->isChecked());
    });

    connect(start_simulation_button, &QPushButton::clicked, this, [=](){

        m_traffic_lights_duration_input_widget->check_input_validity();

        std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> min_max_and_mean_cars_parameters =
            m_cars_parameters_input_widget->get_road_users_min_max_and_mean_basic_parameters();

        std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> min_max_and_mean_pedestrians_parameters =
            m_pedestrians_parameters_input_widget->get_road_users_min_max_and_mean_basic_parameters();

        int cars_to_place = 0;
        int pedestrians_to_place = 0;

        if(automatic_placement_check_box->isChecked()){
            cars_to_place = cars_number_spin_box->value();
            pedestrians_to_place = pedestrians_number_spin_box->value();
        }

        m_simulation_configuration = SSimulationConfiguration(std::get<0>(min_max_and_mean_cars_parameters),
                                                              std::get<2>(min_max_and_mean_cars_parameters),
                                                              std::get<1>(min_max_and_mean_cars_parameters),
                                                              std::get<0>(min_max_and_mean_pedestrians_parameters),
                                                              std::get<2>(min_max_and_mean_pedestrians_parameters),
                                                              std::get<1>(min_max_and_mean_pedestrians_parameters),
                                                              m_traffic_lights_duration_input_widget->get_traffic_lights_duration(),
                                                              automatic_placement_check_box->isChecked(), cars_to_place,
                                                              pedestrians_to_place);
        this->done(QDialog::Accepted);
    });

}

SSimulationConfiguration CSimulationConfigurationDialog::get_simulation_configuration()
{
    return m_simulation_configuration;
}
