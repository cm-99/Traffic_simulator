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
    //Don't serialize filler, it could be probably dropped from CStationaryMapElement hierarchy
    QString serialize_as_string() override {return QString();}
    QString serialize_type_as_string() override {return QString();}
    static CStationaryMapElement * deserialize_from_string([[maybe_unused]] QString item_serialized_to_string) {return nullptr;}
};

#endif // CFILLER_H
