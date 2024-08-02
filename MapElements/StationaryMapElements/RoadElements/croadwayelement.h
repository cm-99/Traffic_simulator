#ifndef CROADWAYELEMENT_H
#define CROADWAYELEMENT_H

#include "MapElements/StationaryMapElements/croadelement.h"

enum ERoadwayElementType {roadway, turn, crossing};

class CRoadwayElement : public CRoadElement
{
public:
    CRoadwayElement(int carriageways_number, int lanes_number, ERoadwayElementType roadway_element_type, EMovementPlane movement_plane, EPermittedRoadUsers permitted_road_users,
                 QString description) :
        CRoadElement(ERoadElementType::roadway_element, movement_plane, permitted_road_users, description),
        m_carriageways_number(carriageways_number),
        m_lanes_number(lanes_number),
        m_roadway_element_type(roadway_element_type) {};

    int get_carriageways_number() const {return m_carriageways_number;}
    int get_lanes_number() const {return m_lanes_number;}
    ERoadwayElementType get_roadway_element_type() const {return m_roadway_element_type;}

private:
    int m_carriageways_number;
    int m_lanes_number;
    ERoadwayElementType m_roadway_element_type;
};

#endif // CROADWAYELEMENT_H
