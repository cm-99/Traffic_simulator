#ifndef CROADUSERSPARAMETERSRANGEINPUTWIDGET_H
#define CROADUSERSPARAMETERSRANGEINPUTWIDGET_H

#include <QFrame>
#include <QSpinBox>

#include "MapElements/croaduser.h"

class CRoadUsersParametersRangeInputWidget : public QFrame
{
public:
    CRoadUsersParametersRangeInputWidget(QString title, ERoadUsers road_users_type);
    CRoadUsersParametersRangeInputWidget(QString title,
                std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters> min_max_and_mean_parameters);

    std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
    get_road_users_min_max_and_mean_basic_parameters();

private:
    QVector<QDoubleSpinBox*> m_min_parameters_spin_boxes;
    QVector<QDoubleSpinBox*> m_max_parameters_spin_boxes;
    QVector<QDoubleSpinBox*> m_mean_parameters_spin_boxes;
};

#endif // CROADUSERSPARAMETERSRANGEINPUTWIDGET_H
