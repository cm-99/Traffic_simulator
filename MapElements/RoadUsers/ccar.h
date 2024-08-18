#ifndef CCAR_H
#define CCAR_H

#include "MapElements/croaduser.h"

/**
 * @brief The CCar class is a graphical and behavioral representation of a car.
 */
class CCar : public CRoadUser
{
public:
    static CRoadUser *create_car();

    virtual void move() override;

private:
    CCar(QPixmap car_pixmap, QString description, EMovementPlane starting_movement_plane);
};

#endif // CCAR_H
