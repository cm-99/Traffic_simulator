#ifndef CROADELEMENT_H
#define CROADELEMENT_H

#include "MapElements/cstationarymapelement.h"

enum EPermittedRoadUsers{cars, pedestrians, all};
enum ERoadElementType{pavement = 0, roadway_element = 1, pedestrian_crossing = 2};
enum EMovementPlane{horizontal, vertical, any};

/**
 * @brief The CRoadElement class is a base class to be inherited by classes representing specific road elements.
 */
class CRoadElement : public CStationaryMapElement
{
public:
    CRoadElement(ERoadElementType road_element_type, EMovementPlane movement_plane, EPermittedRoadUsers permitted_road_users,
                 QString description, QString pixmap_path) :
        CStationaryMapElement(EStationaryMapElementType::road_element, description, true, pixmap_path),
        m_movement_plane(movement_plane),
        m_road_element_type(road_element_type),
        m_permitted_road_users(permitted_road_users) {setZValue(0);}

    virtual QString serialize_as_string()=0;
    virtual QString serialize_type_as_string()=0;

    inline ERoadElementType get_road_element_type() const {return m_road_element_type;}
    inline EMovementPlane get_movement_plane() const {return m_movement_plane;}
    inline EPermittedRoadUsers get_permitted_road_users() const {return m_permitted_road_users;}
    inline void set_movement_plane(EMovementPlane movement_plane) {m_movement_plane = movement_plane;}

private:
    EMovementPlane m_movement_plane;
    ERoadElementType m_road_element_type;
    EPermittedRoadUsers m_permitted_road_users;
};

#endif // CROADELEMENT_H
