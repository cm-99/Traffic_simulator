#ifndef CCROSSING_H
#define CCROSSING_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CCrossing class is a graphical representation of crossing/intersection.
 */
class CCrossing : public CRoadElement
{
public:
    CCrossing(QPixmap crossing_pixmap);
};

#endif // CCROSSING_H
