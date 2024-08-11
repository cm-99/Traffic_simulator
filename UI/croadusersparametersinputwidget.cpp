#include <QFormLayout>

#include "croadusersparametersinputwidget.h"
#include "UI/cexpandablewidget.h"

CRoadUsersParametersInputWidget::CRoadUsersParametersInputWidget(QString title) {
    auto *content_widget = new QFrame(this);
    auto *widget_layout = new QVBoxLayout();
    auto *wrapper_layout = new QFormLayout();
    auto *expandable_widget = new CExpandableWidget(title, 300, this);

    int attributes_number = SRoadUsersBasicParameters::get_attributes_number();
    QStringList attributes_names = SRoadUsersBasicParameters::get_attributes_names_list();
    QStringList attributes_values_specifiers = {"[minimum]", "[maximum]", "[default]"};

    for(int i = 0; i < attributes_number; i++){
        QString attribute_name = attributes_names[i];
        auto min_max_and_default_attribute_value = SRoadUsersBasicParameters::get_attribute_min_max_and_default_value(i);

        for(int j = 0; j < 3; j++){
            QString row_name = attributes_values_specifiers[j] + " " + attribute_name;

            auto attribute_spin_box = new QSpinBox();
            attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
            attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));
            attribute_spin_box->setValue(std::get<2>(min_max_and_default_attribute_value));

            wrapper_layout->addRow(row_name, attribute_spin_box);
        }
    }

    content_widget->setLayout(widget_layout);
    wrapper_layout->addWidget(content_widget);
    content_widget->setMinimumWidth(1000);
    expandable_widget->set_content_layout(*wrapper_layout);

    expandable_widget->slot_toggle(true);
}

SRoadUsersBasicParameters CRoadUsersParametersInputWidget::get_road_users_minimum_basic_parameters()
{

}

SRoadUsersBasicParameters CRoadUsersParametersInputWidget::get_road_users_default_basic_parameters()
{

}

SRoadUsersBasicParameters CRoadUsersParametersInputWidget::get_road_users_maximum_basic_parameters()
{

}
