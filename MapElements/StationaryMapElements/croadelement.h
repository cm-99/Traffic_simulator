#ifndef CROADELEMENT_H
#define CROADELEMENT_H

#include "MapElements/cstationarymapelement.h"

enum class EPermittedRoadUsers{cars, pedestrians, all};
enum class ERoadElementType{pavement, roadway_element, pedestrian_crossing};
enum class EMovementPlane{horizontal, vertical, any};

/**
 * @brief The CRoadElement class is a base class to be inherited by classes representing specific road elements.
 */
class CRoadElement : public CStationaryMapElement
{
public:
    CRoadElement(ERoadElementType road_element_type, EMovementPlane movement_plane, EPermittedRoadUsers permitted_road_users,
                 QString description) :
        CStationaryMapElement(EStationaryMapElementType::road_element, description, true),
        m_movement_plane(movement_plane),
        m_road_element_type(road_element_type),
        m_permitted_road_users(permitted_road_users) {setZValue(0);}

    ERoadElementType get_road_element_type() const {return m_road_element_type;}
    EMovementPlane get_movement_plane() const {return m_movement_plane;}
    EPermittedRoadUsers get_permitted_road_users() const {return m_permitted_road_users;}
    void set_movement_plane(EMovementPlane movement_plane) {m_movement_plane = movement_plane;}

private:
    EMovementPlane m_movement_plane;
    ERoadElementType m_road_element_type;
    EPermittedRoadUsers m_permitted_road_users;
};

#endif // CROADELEMENT_H
