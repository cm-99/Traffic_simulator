#ifndef CPAVEMENT_H
#define CPAVEMENT_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CPavement class is a graphical representation of pedestrian crossing.
 */
class CPavement : public CRoadElement
{
public:
    CPavement(QPixmap pavement_pixmap);
};

#endif // CPAVEMENT_H
