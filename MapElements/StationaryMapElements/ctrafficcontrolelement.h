#ifndef CTRAFFICCONTROLELEMENT_H
#define CTRAFFICCONTROLELEMENT_H

#include "MapElements/cstationarymapelement.h"

enum ETrafficControlElementType {traffic_sign = 0, traffic_lights = 1};

/**
 * @brief The CTrafficControlElement class is a base class for classes representing specific traffic control elements of the simulation
 * included in ETrafficControlElementType.
 */
class CTrafficControlElement : public CStationaryMapElement
{
public:
    CTrafficControlElement(ETrafficControlElementType traffic_control_element_type, QString description, QString pixmap_path) :
        CStationaryMapElement(EStationaryMapElementType::traffic_control_element, description, false, pixmap_path),
        m_traffic_control_element_type(traffic_control_element_type) { setZValue(2);}
    inline ETrafficControlElementType get_traffic_control_element_type() const {return m_traffic_control_element_type;}

    virtual QString serialize_as_string()=0;
    virtual QString serialize_type_as_string()=0;

private:
    ETrafficControlElementType m_traffic_control_element_type;
};

#endif // CTRAFFICCONTROLELEMENT_H
