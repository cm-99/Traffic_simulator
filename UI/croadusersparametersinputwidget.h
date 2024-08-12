#ifndef CROADUSERSPARAMETERSINPUTWIDGET_H
#define CROADUSERSPARAMETERSINPUTWIDGET_H

#include <QFrame>
#include <QSpinBox>

#include "MapElements/croaduser.h"

class CRoadUsersParametersInputWidget : public QFrame
{
public:
    CRoadUsersParametersInputWidget(QString title);
    std::tuple<SRoadUsersBasicParameters, SRoadUsersBasicParameters, SRoadUsersBasicParameters>
    get_road_users_min_max_and_mean_basic_parameters();

private:
    QVector<QSpinBox*> m_min_parameters_spin_boxes;
    QVector<QSpinBox*> m_max_parameters_spin_boxes;
    QVector<QSpinBox*> m_mean_parameters_spin_boxes;
};

#endif // CROADUSERSPARAMETERSINPUTWIDGET_H
