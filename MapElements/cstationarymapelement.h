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
    CStationaryMapElement(EStationaryMapElementType map_element_type, QString description, bool is_rotable):
        m_is_rotable(is_rotable),
        m_map_element_type(map_element_type),
        m_description(description) {};
    EStationaryMapElementType get_map_element_type() const {return m_map_element_type;}
    static QSize get_default_map_element_size() {return QSize(40, 40);}
    QString get_description() const {return m_description;}
    bool is_rotable() const {return m_is_rotable;}

private:
    bool m_is_rotable;
    EStationaryMapElementType m_map_element_type;
    QString m_description;
};

#endif // CSTATIONARYMAPELEMENT_H
