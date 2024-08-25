#include "csimulationpage.h"
#include "UI/ctrafficlightsdurationinputwidget.h"
#include "UI/croadusersparametersinputwidget.h"
#include "cmainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QCheckBox>

CSimulationPage::CSimulationPage(CSimulationController *simulation_controller, QWidget *parent) :
    m_simulation_controller(simulation_controller)
{
    this->setParent(parent);
    auto main_layout = new QVBoxLayout();
    this->setLayout(main_layout);
    auto road_users_placement_widget = new CRoadUsersPlacementWidget(this);

    m_speed_button_icons.append(new QIcon(QPixmap(":/graphics/buttons_icons/game_speed_1_button.png")));
    m_speed_button_icons.append(new QIcon(QPixmap(":/graphics/buttons_icons/game_speed_2_button.png")));
    m_speed_button_icons.append(new QIcon(QPixmap(":/graphics/buttons_icons/game_speed_3_button.png")));

    QHBoxLayout *simulation_buttons_layout = new QHBoxLayout();

    m_start_button = new QPushButton(this);
    QPixmap start_pixmap(":/graphics/buttons_icons/play_icon.jpg");
    m_start_button->setFixedSize(60, 60);
    QIcon icon(start_pixmap);
    m_start_button->setIcon(icon);
    m_start_button->setIconSize(QSize(60, 60));

    m_simulation_speed_button = new QPushButton(this);
    m_simulation_speed_button->setFixedSize(60, 60);
    m_simulation_speed_button->setIcon(*m_speed_button_icons[0]);
    m_simulation_speed_button->setIconSize(QSize(55, 55));

    m_restart_simulation_button = new QPushButton(this);
    QPixmap restart_pixmap(":/graphics/buttons_icons/restart_button.png");
    m_restart_simulation_button->setFixedSize(60, 60);
    QIcon restart_icon(restart_pixmap);
    m_restart_simulation_button->setIcon(restart_icon);
    m_restart_simulation_button->setIconSize(QSize(60, 60));
    m_restart_simulation_button->setToolTip("Restart simulation");

    m_reconfigure_simulation_button = new QPushButton("Reconfigure simulation", this);

    connect(m_simulation_speed_button, &QPushButton::clicked, this, &CSimulationPage::slot_change_simulation_speed);
    connect(m_start_button, &QPushButton::clicked, this, &CSimulationPage::slot_change_simulation_state);
    connect(m_restart_simulation_button, &QPushButton::clicked, this, &CSimulationPage::slot_restart_simulation);
    connect(m_reconfigure_simulation_button, &QPushButton::clicked, this, &CSimulationPage::slot_reconfigure_simulation);
    connect(m_simulation_controller, &CSimulationController::signal_traffic_light_configuration_requested, this, &CSimulationPage::slot_request_traffic_light_configuration);
    connect(m_simulation_controller, &CSimulationController::signal_road_user_configuration_requested, this, &CSimulationPage::slot_request_road_users_parameters);

    int height_hint = m_start_button->sizeHint().height();
    simulation_buttons_layout->addWidget(m_start_button);
    simulation_buttons_layout->addWidget(m_simulation_speed_button);
    simulation_buttons_layout->addWidget(m_restart_simulation_button);
    simulation_buttons_layout->insertSpacerItem(3, new QSpacerItem(0, height_hint, QSizePolicy::Expanding, QSizePolicy::Fixed));
    simulation_buttons_layout->addWidget(m_reconfigure_simulation_button);

    main_layout->addWidget(m_simulation_controller->get_view());
    main_layout->addWidget(road_users_placement_widget);
    main_layout->addLayout(simulation_buttons_layout);
}

CSimulationPage::~CSimulationPage()
{
    for(auto icon : m_speed_button_icons){
        delete icon;
    }
    m_speed_button_icons.clear();
}

void CSimulationPage::restore_to_default()
{
    if(m_simulation_was_started && !m_simulation_is_paused){
        m_simulation_controller->pause_simulation();
        m_simulation_is_paused = true;
    }

    m_simulation_speed = 1;
    m_simulation_controller->set_simulation_speed(1);

    m_simulation_speed_button->setIcon(*m_speed_button_icons[0]);
    m_simulation_speed_button->setIconSize(QSize(55, 55));

    QPixmap start_pixmap(":/graphics/buttons_icons/play_icon.jpg");
    m_start_button->setFixedSize(60, 60);
    QIcon icon(start_pixmap);
    m_start_button->setIcon(icon);
    m_start_button->setIconSize(QSize(60, 60));
}

void CSimulationPage::relay_creator_button_clicked(CRoadUser *(*creation_func)())
{
    m_simulation_controller->set_user_to_place_creation_func(creation_func);
}

void CSimulationPage::slot_change_simulation_speed()
{
    if(m_simulation_speed < 3){
        m_simulation_speed++;
    }
    else{
        m_simulation_speed = 1;
    }

    m_simulation_speed_button->setIcon(*m_speed_button_icons[m_simulation_speed -1 ]);
    m_simulation_speed_button->setIconSize(QSize(55, 55));
    m_simulation_controller->set_simulation_speed(m_simulation_speed);
}

void CSimulationPage::slot_change_simulation_state()
{
    if(!m_simulation_controller->simulation_is_ready()){
        QMessageBox::information(this, "", "Place at least one road user to enable simulation.");
    }
    else{
        if(!m_simulation_was_started || m_simulation_is_paused){
            if(!m_simulation_was_started){
                m_simulation_was_started = true;
                m_simulation_controller->start_simulation();
            }
            else{
                m_simulation_controller->resume_simulation();
                m_simulation_is_paused = false;
            }

            QPixmap start_pixmap(":/graphics/buttons_icons/pause_button.png");
            QIcon icon(start_pixmap);
            m_start_button->setIcon(icon);
            m_start_button->setIconSize(QSize(60, 60));
        }
        else{
            m_simulation_controller->pause_simulation();
            QPixmap start_pixmap(":/graphics/buttons_icons/play_icon.jpg");
            QIcon icon(start_pixmap);
            m_start_button->setIcon(icon);
            m_start_button->setIconSize(QSize(60, 60));
            m_simulation_is_paused = true;
        }
    }
}

void CSimulationPage::slot_restart_simulation()
{
    m_simulation_controller->restart_simulation();
    m_simulation_is_paused = false;
    m_simulation_was_started = false;
}

void CSimulationPage::slot_reconfigure_simulation()
{
    SSimulationConfiguration config = static_cast<CMainWindow*>(this->parent())->get_simulation_configuration_from_user();
    if(config.is_empty()){
        return;
    }

    m_simulation_controller->set_simulation_configuration(config);
    m_simulation_is_paused = false;
    m_simulation_was_started = false;
}

void CSimulationPage::slot_request_traffic_light_configuration(STrafficLightsDuration traffic_lights_duration, bool automatic_offset_is_disabled, bool is_disabled)
{
    QDialog dialog;
    QVBoxLayout dialog_layout;
    CTrafficLightsDurationInputWidget traffic_lights_duration_widget(traffic_lights_duration);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog_layout.addWidget(&traffic_lights_duration_widget);

    QSpinBox *offset_spin_box = nullptr;
    if(automatic_offset_is_disabled){
        QHBoxLayout offset_layout;
        QLabel offset_label("Time offset");
        offset_spin_box = new QSpinBox();
        offset_spin_box->setMinimum(0);
        offset_layout.addWidget(&offset_label);
        offset_layout.addWidget(offset_spin_box);
        dialog_layout.addLayout(&offset_layout);
    }

    QHBoxLayout light_disabled_layout;
    QLabel light_disabled_label("Disable light");
    QCheckBox *light_disabled_check_box = new QCheckBox();
    light_disabled_check_box->setChecked(is_disabled);

    light_disabled_layout.addWidget(&light_disabled_label);
    light_disabled_layout.addWidget(light_disabled_check_box);
    dialog_layout.addLayout(&light_disabled_layout);

    dialog_layout.addWidget(buttonBox);
    dialog.setLayout(&dialog_layout);

    if(dialog.exec() == QDialog::DialogCode::Accepted){
        int offset = -1;
        if(automatic_offset_is_disabled){
            offset = offset_spin_box->value();
        }

        m_simulation_controller->configure_traffic_light(traffic_lights_duration_widget.get_traffic_lights_duration_in_sec(),
                                                         offset, light_disabled_check_box->isChecked());
    }
    else{
        m_simulation_controller->configure_traffic_light(STrafficLightsDuration(),
                                                         0, light_disabled_check_box->isChecked());
    }
}

void CSimulationPage::slot_request_road_users_parameters(SRoadUsersBasicParameters parameters, ERoadUsers type)
{
    QDialog dialog;
    QVBoxLayout dialog_layout;
    CRoadUsersParametersInputWidget parameters_input_widget(parameters, type);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog_layout.addWidget(&parameters_input_widget);
    dialog_layout.addWidget(buttonBox);
    dialog.setLayout(&dialog_layout);

    if(dialog.exec() == QDialog::DialogCode::Accepted){
        m_simulation_controller->configure_road_user(parameters_input_widget.get_road_users_parameters());
    }
    else{
        m_simulation_controller->configure_road_user(SRoadUsersBasicParameters());
    }
}
