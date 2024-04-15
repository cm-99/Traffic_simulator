#include "cpedestrian.h"

CPedestrian::CPedestrian(SRoadUsersBasicParameters basic_parameters, QPixmap pedestrian_pixmap) :
    CRoadUser(ERoadUsers::pedestrian)
{
    m_road_users_parameters = basic_parameters;
    setPixmap(pedestrian_pixmap);
}

void CPedestrian::move()
{

}
