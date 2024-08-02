#ifndef CPAVEMENT_H
#define CPAVEMENT_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CPavement class is a graphical representation of pavement.
 */
class CPavement : public CRoadElement
{
public:
    static CStationaryMapElement *create_pavement();

private:
    CPavement(QPixmap pavement_pixmap, QString description, EMovementPlane movement_plane);
};

#endif // CPAVEMENT_H
