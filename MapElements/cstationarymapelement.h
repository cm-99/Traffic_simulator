#ifndef CSTATIONARYMAPELEMENT_H
#define CSTATIONARYMAPELEMENT_H

#include <QGraphicsPixmapItem>
#include <QSize>

enum class EStationaryMapElementType {road_element, traffic_control_element, filler};

/**
 * @brief The CStationaryMapElement class is a base class for all the classes representing specific stationary elements of the simulation map
 * included in EStationaryMapElementType.
 */
class CStationaryMapElement: public QGraphicsPixmapItem
{
public:
    CStationaryMapElement(EStationaryMapElementType map_element_type = EStationaryMapElementType::filler)
        : m_map_element_type(map_element_type){};
    EStationaryMapElementType get_map_element_type() const {return m_map_element_type;}
    const QSize& get_default_map_element_size() const {return m_default_map_element_size;}

private:
    QSize m_default_map_element_size{QSize(40, 40)};
    EStationaryMapElementType m_map_element_type;
};

#endif // CSTATIONARYMAPELEMENT_H
