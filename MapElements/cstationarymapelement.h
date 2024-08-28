#ifndef CSTATIONARYMAPELEMENT_H
#define CSTATIONARYMAPELEMENT_H

#include <QGraphicsPixmapItem>
#include <QSize>

enum EStationaryMapElementType {road_element = 0, traffic_control_element = 1, filler = 2};

/**
 * @brief The CStationaryMapElement class is a base class for all the classes representing specific stationary elements of the simulation map
 * included in EStationaryMapElementType.
 */
class CStationaryMapElement: public QGraphicsPixmapItem
{
public:
    CStationaryMapElement(EStationaryMapElementType map_element_type, QString description, bool is_rotable, QString pixmap_path):
        m_pixmap_path(pixmap_path),
        m_is_rotable(is_rotable),
        m_map_element_type(map_element_type),
        m_description(description) {};

    /**
     * @brief To be implemented as serialization of the item to the string format. It should include all parameters needed to restore
     * map element to previous state.
     * @return
     */
    virtual QString serialize_as_string()=0;
    /**
     * @brief To be implemented as type serialization to the string format.
     * @return
     */
    virtual QString serialize_type_as_string()=0;

    inline EStationaryMapElementType get_map_element_type() const {return m_map_element_type;}
    static inline QSize get_default_map_element_size() {return QSize(40, 40);}
    inline QString get_description() const {return m_description;}
    inline QString get_pixmap_path() const {return m_pixmap_path;}
    inline bool is_rotable() const {return m_is_rotable;}
    inline bool is_placement_correction_pending() const {return m_is_placement_correction_pending;}

    inline void set_is_placement_correction_pending(bool is_placement_correction_pending)
    {m_is_placement_correction_pending = is_placement_correction_pending;}

protected:
    QString m_pixmap_path;

private:
    bool m_is_rotable;
    bool m_is_placement_correction_pending{false};
    EStationaryMapElementType m_map_element_type;
    QString m_description;
};

#endif // CSTATIONARYMAPELEMENT_H
