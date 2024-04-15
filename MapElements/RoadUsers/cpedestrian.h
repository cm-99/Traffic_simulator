#ifndef CPEDESTRIAN_H
#define CPEDESTRIAN_H

#include "MapElements/croaduser.h"

/**
 * @brief The CPedestrian class is a graphical and behavioral representation of a pedestrian.
 */
class CPedestrian : public CRoadUser
{
public:
    CPedestrian(SRoadUsersBasicParameters basic_parameters, QPixmap pedestrian_pixmap);
    virtual void move() override;
};

#endif // CPEDESTRIAN_H
