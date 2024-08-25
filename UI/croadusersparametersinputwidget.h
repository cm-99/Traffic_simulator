#ifndef CROADUSERSPARAMETERSINPUTWIDGET_H
#define CROADUSERSPARAMETERSINPUTWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>

#include "MapElements/croaduser.h"

class CRoadUsersParametersInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRoadUsersParametersInputWidget(SRoadUsersBasicParameters parameters, ERoadUsers type);
    SRoadUsersBasicParameters get_road_users_parameters();

private:
    QVector<QDoubleSpinBox*> m_parameters_spin_boxes;

};

#endif // CROADUSERSPARAMETERSINPUTWIDGET_H
