#ifndef CROADUSERSPARAMETERSINPUTWIDGET_H
#define CROADUSERSPARAMETERSINPUTWIDGET_H

#include <QFrame>
#include <QSpinBox>

#include "MapElements/croaduser.h"

class CRoadUsersParametersInputWidget : public QFrame
{
public:
    CRoadUsersParametersInputWidget(QString title);
    SRoadUsersBasicParameters get_road_users_minimum_basic_parameters();
    SRoadUsersBasicParameters get_road_users_default_basic_parameters();
    SRoadUsersBasicParameters get_road_users_maximum_basic_parameters();
};

#endif // CROADUSERSPARAMETERSINPUTWIDGET_H
