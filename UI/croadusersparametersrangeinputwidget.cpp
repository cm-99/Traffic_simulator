#include <QFormLayout>
#include <QLabel>

#include "croadusersparametersrangeinputwidget.h"

CRoadUsersParametersRangeInputWidget::CRoadUsersParametersRangeInputWidget(QString title, ERoadUsers road_users_type) {
    auto main_layout = new QVBoxLayout(this);
    main_layout->addWidget(new QLabel(title));

    auto wrapper_layout = new QHBoxLayout();
    auto wrapper_widget = new QWidget();
    auto left_layout = new QFormLayout();
    auto right_layout = new QFormLayout();
    auto left_widget = new QWidget();
    auto right_widget = new QWidget();
    left_widget->setLayout(left_layout);
    right_widget->setLayout(right_layout);
    wrapper_layout->addWidget(left_widget);
    wrapper_layout->addWidget(right_widget);
    wrapper_widget->setLayout(wrapper_layout);
    main_layout->addWidget(wrapper_widget);

    int attributes_number = SRoadUsersBasicParameters::get_attributes_number();
    QStringList attributes_names = SRoadUsersBasicParameters::get_attributes_names_list();
    QStringList attributes_values_specifiers = {"[minimum]", "[mean]", "[maximum]"};

    for(int i = 0; i < attributes_number; i++){
        QString attribute_name = attributes_names[i];
        auto min_max_and_default_attribute_value =
            SRoadUsersBasicParameters::get_road_users_attributes_min_max_and_mean_values(i, road_users_type);
        auto attribute_label = new QLabel(attribute_name);
        attribute_label->setMinimumWidth(attribute_label->sizeHint().width());

        if(i < (attributes_number + 1)/2){
            left_layout->addRow(attribute_label);
        }
        else{
            right_layout->addRow(attribute_label);
        }
    
        for(int j = 0; j < 3; j++){

            QString row_specifier = attributes_values_specifiers[j];
            auto attribute_spin_box = new QDoubleSpinBox();

            attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
            attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));

            if(i < (attributes_number + 1)/2){
                left_layout->addRow(row_specifier, attribute_spin_box);
            }
            else{
                right_layout->addRow(row_specifier, attribute_spin_box);
            }

            switch (j){
            case 0:
                m_min_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<0>(min_max_and_default_attribute_value));
                break;
            case 1:
                m_mean_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<2>(min_max_and_default_attribute_value));
                break;
            case 2:
                m_max_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<1>(min_max_and_default_attribute_value));
                break;
            }
        }

        connect(m_min_parameters_spin_boxes[i], &QDoubleSpinBox::valueChanged, m_mean_parameters_spin_boxes[i], &QDoubleSpinBox::setMinimum);
        connect(m_mean_parameters_spin_boxes[i], &QDoubleSpinBox::valueChanged, m_max_parameters_spin_boxes[i], &QDoubleSpinBox::setMinimum);
    }

    this->setLayout(main_layout);
}

CRoadUsersParametersRangeInputWidget::CRoadUsersParametersRangeInputWidget(QString title,
                                                                 std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> min_max_and_mean_parameters)
{
    auto main_layout = new QVBoxLayout(this);
    main_layout->addWidget(new QLabel(title));

    auto wrapper_layout = new QHBoxLayout();
    auto wrapper_widget = new QWidget();
    auto left_layout = new QFormLayout();
    auto right_layout = new QFormLayout();
    auto left_widget = new QWidget();
    auto right_widget = new QWidget();
    left_widget->setLayout(left_layout);
    right_widget->setLayout(right_layout);
    wrapper_layout->addWidget(left_widget);
    wrapper_layout->addWidget(right_widget);
    wrapper_widget->setLayout(wrapper_layout);
    main_layout->addWidget(wrapper_widget);

    int attributes_number = SRoadUsersBasicParameters::get_attributes_number();
    QStringList attributes_names = SRoadUsersBasicParameters::get_attributes_names_list();
    QStringList attributes_values_specifiers = {"[minimum]", "[mean]", "[maximum]"};

    for(int i = 0; i < attributes_number; i++){
        QString attribute_name = attributes_names[i];
        auto min_max_and_default_attribute_value =
        std::tuple<int, int, int>(std::get<0>(min_max_and_mean_parameters).get_attribute_by_index(i),
                                    std::get<1>(min_max_and_mean_parameters).get_attribute_by_index(i),
                                    std::get<2>(min_max_and_mean_parameters).get_attribute_by_index(i));

        auto attribute_label = new QLabel(attribute_name);
        attribute_label->setMinimumWidth(attribute_label->sizeHint().width());

        if(i < (attributes_number + 1)/2){
            left_layout->addRow(attribute_label);
        }
        else{
            right_layout->addRow(attribute_label);
        }

        for(int j = 0; j < 3; j++){

            QString row_specifier = attributes_values_specifiers[j];
            auto attribute_spin_box = new QDoubleSpinBox();

            attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
            attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));

            if(i < (attributes_number + 1)/2){
                left_layout->addRow(row_specifier, attribute_spin_box);
            }
            else{
                right_layout->addRow(row_specifier, attribute_spin_box);
            }

            switch (j){
            case 0:
                m_min_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<0>(min_max_and_default_attribute_value));
                break;
            case 1:
                m_mean_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<2>(min_max_and_default_attribute_value));
                break;
            case 2:
                m_max_parameters_spin_boxes.append(attribute_spin_box);
                attribute_spin_box->setValue(std::get<1>(min_max_and_default_attribute_value));
                break;
            }
        }

        connect(m_min_parameters_spin_boxes[i], &QDoubleSpinBox::valueChanged, m_mean_parameters_spin_boxes[i], &QDoubleSpinBox::setMinimum);
        connect(m_mean_parameters_spin_boxes[i], &QDoubleSpinBox::valueChanged, m_max_parameters_spin_boxes[i], &QDoubleSpinBox::setMinimum);
    }

    this->setLayout(main_layout);
}

std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> CRoadUsersParametersRangeInputWidget::get_road_users_min_max_and_mean_basic_parameters()
{
    QVector<double> min_parameters_values;
    QVector<double> max_parameters_values;
    QVector<double> mean_parameters_values;

    int spin_boxes_count = m_max_parameters_spin_boxes.size();
    for(int i = 0; i < spin_boxes_count; i++){
        min_parameters_values.append(m_min_parameters_spin_boxes[i]->value());
        max_parameters_values.append(m_max_parameters_spin_boxes[i]->value());
        mean_parameters_values.append(m_mean_parameters_spin_boxes[i]->value());
    }

    return std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
        (SRoadUsersBasicParameters(min_parameters_values), SRoadUsersBasicParameters(max_parameters_values),
        SRoadUsersBasicParameters(mean_parameters_values));

}
