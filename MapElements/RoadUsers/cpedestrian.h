#ifndef CPEDESTRIAN_H
#define CPEDESTRIAN_H

#include "MapElements/croaduser.h"

/**
 * @brief The CPedestrian class is a graphical and behavioral representation of a pedestrian.
 */
class CPedestrian : public CRoadUser
{
public:
    static CRoadUser *create_pedestrian();
    virtual void move() override;

private:
    /**
     * @brief CPedestrian
     * @param pedestrian_states_pixmaps - vector should contain 3 pixmaps in order - stationary pedestrian, pedestrian taking a step
     * from one leg and pedestrian taking a step from another leg.
     */
    CPedestrian(QVector<QPixmap> pedestrian_states_pixmaps, QString description, EMovementPlane starting_movement_plane);
    QVector<QPixmap> m_pedestrian_states_pixmaps;

};

#endif // CPEDESTRIAN_H
