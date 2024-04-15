#ifndef CFILLER_H
#define CFILLER_H

#include "MapElements/cstationarymapelement.h"

/**
 * @brief The CFiller class is a graphical representation of a rectangle.
 *
 * It should not allow the movement of road users, it is a generalized representation of objects outside the road.
 */
class CFiller : public CStationaryMapElement
{
public:
    CFiller();
};

#endif // CFILLER_H
