#include "croadusersparametersinputwidget.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

CRoadUsersParametersInputWidget::CRoadUsersParametersInputWidget(SRoadUsersBasicParameters parameters, ERoadUsers type)
{
    auto wrapper_layout = new QHBoxLayout();
    auto left_layout = new QFormLayout();
    auto right_layout = new QFormLayout();
    auto left_widget = new QWidget();
    auto right_widget = new QWidget();
    left_widget->setLayout(left_layout);
    right_widget->setLayout(right_layout);
    wrapper_layout->addWidget(left_widget);
    wrapper_layout->addWidget(right_widget);

    int attributes_number = SRoadUsersBasicParameters::get_attributes_number();
    QStringList attributes_names = SRoadUsersBasicParameters::get_attributes_names_list();

    for(int i = 0; i < attributes_number; i++){
        QString attribute_name = attributes_names[i];
        auto min_max_and_default_attribute_value =
            SRoadUsersBasicParameters::get_road_users_attributes_min_max_and_mean_values(i, type);

        auto attribute_spin_box = new QDoubleSpinBox();

        attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
        attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));
        attribute_spin_box->setValue(parameters.get_attribute_by_index(i));

        if(i < (attributes_number + 1)/2){
            left_layout->addRow(attribute_name, attribute_spin_box);
        }
        else{
            right_layout->addRow(attribute_name, attribute_spin_box);
        }
    }

    this->setLayout(wrapper_layout);
}

SRoadUsersBasicParameters CRoadUsersParametersInputWidget::get_road_users_parameters()
{
    QVector<double> parameters_values;

    for(int i = 0; i < m_parameters_spin_boxes.size(); i++){
        parameters_values.append(m_parameters_spin_boxes[i]->value());
    }

    return SRoadUsersBasicParameters(parameters_values);
}
