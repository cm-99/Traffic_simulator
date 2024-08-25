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

    virtual void move(CReadOnlyMap *map) override;
    bool get_is_braking() const {return m_is_braking;}
    bool get_is_waiting() const {return m_is_waiting;}

private:
    CCar(QPixmap car_pixmap, QString description, EMovementPlane movement_plane,
         EHorizontalMoveDirection horizontal_move_direction = EHorizontalMoveDirection::left,
         EVerticalMoveDirection vertical_move_direction = EVerticalMoveDirection::up);

    bool m_is_braking{false};
    bool m_is_waiting{false};
};

#endif // CCAR_H
