#ifndef CROADELEMENT_H
#define CROADELEMENT_H

#include "MapElements/cstationarymapelement.h"

enum class ERoadElementType{pavement, roadway, pedestrian_crossing, crossing};

/**
 * @brief The CRoadElement class is a base class to be inherited by classes representing specific road elements.
 */
class CRoadElement : public CStationaryMapElement
{
public:
    CRoadElement(ERoadElementType road_element_type) :
        m_road_element_type(road_element_type) {setZValue(0);}
    ERoadElementType get_road_element_type() const {return m_road_element_type;}

private:
    ERoadElementType m_road_element_type;
};

#endif // CROADELEMENT_H
