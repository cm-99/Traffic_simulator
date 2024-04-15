#include "croadway.h"

CRoadway::CRoadway(int carriageways_number, int lanes_numer, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap) :
    CRoadElement(ERoadElementType::roadway)
{
    m_carriageways_number = carriageways_number;
    m_lanes_numer = lanes_numer;
    m_roadway_surface_marking = roadway_surface_marking;

    setPixmap(roadway_pixmap);
    this -> setZValue(-1);
}
