#ifndef CTRAFFICCONTROLELEMENT_H
#define CTRAFFICCONTROLELEMENT_H

#include "MapElements/cstationarymapelement.h"

enum class ETrafficControlElementType {traffic_sign, traffic_lights};

/**
 * @brief The CTrafficControlElement class is a base class for classes representing specific traffic control elements of the simulation
 * included in ETrafficControlElementType.
 */
class CTrafficControlElement : public CStationaryMapElement
{
public:
    CTrafficControlElement(ETrafficControlElementType traffic_control_element_type, QString description) :
        CStationaryMapElement(EStationaryMapElementType::traffic_control_element, description, false),
        m_traffic_control_element_type(traffic_control_element_type) { setZValue(2);}
    ETrafficControlElementType get_traffic_control_element_type() const {return m_traffic_control_element_type;}

private:
    ETrafficControlElementType m_traffic_control_element_type;
};

#endif // CTRAFFICCONTROLELEMENT_H
