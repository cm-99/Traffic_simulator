#include <QFormLayout>
#include <QLabel>

#include "croadusersparametersinputwidget.h"
#include "UI/cexpandablewidget.h"

CRoadUsersParametersInputWidget::CRoadUsersParametersInputWidget(QString title, ERoadUsers road_users_type) {
    auto *content_layout = new QFormLayout(this);

    int attributes_number = SRoadUsersBasicParameters::get_attributes_number();
    QStringList attributes_names = SRoadUsersBasicParameters::get_attributes_names_list();
    QStringList attributes_values_specifiers = {"[minimum]", "[mean]", "[maximum]"};

    for(int i = 0; i < attributes_number; i++){
        QString attribute_name = attributes_names[i];
        auto min_max_and_default_attribute_value =
            SRoadUsersBasicParameters::get_road_users_attributes_min_max_and_mean_values(i, road_users_type);
        auto attribute_label = new QLabel(attribute_name);
        attribute_label->setMinimumWidth(attribute_label->sizeHint().width());
        content_layout->addRow(attribute_label);
    
        for(int j = 0; j < 3; j++){

            QString row_specifier = attributes_values_specifiers[j];
            auto attribute_spin_box = new QDoubleSpinBox();

            attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
            attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));

            content_layout->addRow(row_specifier, attribute_spin_box);
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

    auto expandable_widget = new CExpandableWidget(title, 300, this);
    expandable_widget->set_content_layout(*content_layout);
    auto wrapper_layout = new QVBoxLayout(this);
    wrapper_layout->addWidget(expandable_widget);

    this->setLayout(wrapper_layout);
    expandable_widget->slot_toggle(true);
}

CRoadUsersParametersInputWidget::CRoadUsersParametersInputWidget(QString title,
                                                                 std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> min_max_and_mean_parameters)
{
    auto *content_layout = new QFormLayout(this);

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
        content_layout->addRow(attribute_label);

        for(int j = 0; j < 3; j++){

            QString row_specifier = attributes_values_specifiers[j];
            auto attribute_spin_box = new QDoubleSpinBox();

            attribute_spin_box->setMinimum(std::get<0>(min_max_and_default_attribute_value));
            attribute_spin_box->setMaximum(std::get<1>(min_max_and_default_attribute_value));

            content_layout->addRow(row_specifier, attribute_spin_box);
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

    auto expandable_widget = new CExpandableWidget(title, 300, this);
    expandable_widget->set_content_layout(*content_layout);
    auto wrapper_layout = new QVBoxLayout(this);
    wrapper_layout->addWidget(expandable_widget);

    this->setLayout(wrapper_layout);
    expandable_widget->slot_toggle(true);
}

std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> CRoadUsersParametersInputWidget::get_road_users_min_max_and_mean_basic_parameters()
{
    QVector<double> min_parameters_values;
    QVector<double> max_parameters_values;
    QVector<double> mean_parameters_values;

    int spin_boxes_count = m_max_parameters_spin_boxes.size();
    for(int i = 0; i < spin_boxes_count; i++){
        min_parameters_values.append(m_min_parameters_spin_boxes[0]->value());
        max_parameters_values.append(m_max_parameters_spin_boxes[0]->value());
        mean_parameters_values.append(m_mean_parameters_spin_boxes[0]->value());
    }

    return std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
        (SRoadUsersBasicParameters(min_parameters_values), SRoadUsersBasicParameters(max_parameters_values),
        SRoadUsersBasicParameters(mean_parameters_values));

}
