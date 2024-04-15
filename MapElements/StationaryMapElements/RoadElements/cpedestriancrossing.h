#ifndef CPEDESTRIANCROSSING_H
#define CPEDESTRIANCROSSING_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CPedestrianCrossing class is a graphical representation of pedestrian crossing.
 */
class CPedestrianCrossing : public CRoadElement
{
public:
    CPedestrianCrossing(QPixmap pedestrian_crossing_pixmap);
};

#endif // CPEDESTRIANCROSSING_H
