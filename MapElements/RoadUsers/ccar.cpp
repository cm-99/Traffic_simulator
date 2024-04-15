#include "ccar.h"

CCar::CCar(SRoadUsersBasicParameters basic_parameters, QPixmap car_pixmap) :
    CRoadUser(ERoadUsers::car)
{
    m_road_users_parameters = basic_parameters;
    setPixmap(car_pixmap);
}

void CCar::move()
{

}
