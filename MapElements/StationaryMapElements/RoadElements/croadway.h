#ifndef CROADWAY_H
#define CROADWAY_H

#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"

/**
 * @brief The ERoadSurfaceMarking enum stores roadway markings to enable implementation of lane changing, turning and overtaking
 */
enum class ERoadSurfaceMarking {none, single_dotted_line, single_continuous, double_continuous_line};

/**
 * @brief The CRoadway class can be used to represent different types of roadways depending on initialization arguments.
 */
class CRoadway : public CRoadwayElement
{
public:
    // POSSIBILITY create lines as graphical masks and switch to builder pattern e.g. create_two_way_roadway().with_dotted_line()
    static CStationaryMapElement *create_one_way_roadway();
    static CStationaryMapElement *create_two_way_roadway_with_dotted_line();
    static CStationaryMapElement *create_two_way_roadway_with_double_continous_line();

    ERoadSurfaceMarking get_roadway_marking() const {return m_roadway_surface_marking;}

private:
    CRoadway(int carriageways_number, int lanes_number, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap,
             QString description, EMovementPlane movement_plane);
    ERoadSurfaceMarking m_roadway_surface_marking;
};

#endif // CROADWAY_H
