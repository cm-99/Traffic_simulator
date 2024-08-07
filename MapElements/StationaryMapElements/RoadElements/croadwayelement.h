#ifndef CROADWAYELEMENT_H
#define CROADWAYELEMENT_H

#include "MapElements/StationaryMapElements/croadelement.h"

enum ERoadwayElementType {roadway = 0, turn = 1, crossing = 2};

class CRoadwayElement : public CRoadElement
{
public:
    CRoadwayElement(int carriageways_number, int lanes_number, ERoadwayElementType roadway_element_type, EMovementPlane movement_plane, EPermittedRoadUsers permitted_road_users,
                 QString description, QString pixmap_path) :
        CRoadElement(ERoadElementType::roadway_element, movement_plane, permitted_road_users, description, pixmap_path),
        m_carriageways_number(carriageways_number),
        m_lanes_number(lanes_number),
        m_roadway_element_type(roadway_element_type) {};

    virtual QString serialize_as_string() = 0;
    virtual QString serialize_type_as_string() = 0;

    inline int get_carriageways_number() const {return m_carriageways_number;}
    inline int get_lanes_number() const {return m_lanes_number;}
    inline ERoadwayElementType get_roadway_element_type() const {return m_roadway_element_type;}

private:
    int m_carriageways_number;
    int m_lanes_number;
    ERoadwayElementType m_roadway_element_type;
};

#endif // CROADWAYELEMENT_H
