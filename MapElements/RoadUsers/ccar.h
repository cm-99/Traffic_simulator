#ifndef CCAR_H
#define CCAR_H

#include "MapElements/croaduser.h"

/**
 * @brief The CCar class is a graphical and behavioral representation of a car.
 */
class CCar : public CRoadUser
{
public:
    CCar(SRoadUsersBasicParameters basic_parameters, QPixmap car_pixmap);
    virtual void move() override;
};

#endif // CCAR_H
