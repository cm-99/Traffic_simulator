#ifndef CROADWAY_H
#define CROADWAY_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The ERoadSurfaceMarking enum stores horizontal roadway markings to enable implementation of lane changes, turning and overtaking
 */
enum class ERoadSurfaceMarking {none, single_dotted_line, single_continuous, double_continuous_line};

/**
 * @brief The CRoadway class can be used to represent different types of roadways depending on initialization arguments.
 */
class CRoadway : public CRoadElement
{
public:
    CRoadway(int carriageways_number, int lanes_numer, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap);

    int get_carriageways_numer() const {return m_carriageways_number;}
    int get_lanes_number() const {return m_lanes_numer;}
    ERoadSurfaceMarking get_roadway_marking() const {return m_roadway_surface_marking;}

private:
    int m_carriageways_number;
    int m_lanes_numer;
    ERoadSurfaceMarking m_roadway_surface_marking;
};

#endif // CROADWAY_H
