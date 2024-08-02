#include "croadway.h"

CRoadway::CRoadway(int carriageways_number, int lanes_number, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap, QString description, EMovementPlane movement_plane) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::roadway, movement_plane, EPermittedRoadUsers::cars, description),
    m_roadway_surface_marking(roadway_surface_marking)
{
    setPixmap(roadway_pixmap);
}

CStationaryMapElement *CRoadway::create_one_way_roadway()
{
    return new CRoadway(1, 1, ERoadSurfaceMarking::none, QPixmap(":/map_elements_graphics/roadways/one_way_roadway.png"),
                        "One way roadway", EMovementPlane::vertical);
}

CStationaryMapElement *CRoadway::create_two_way_roadway_with_dotted_line()
{
    return new CRoadway(2, 1, ERoadSurfaceMarking::single_dotted_line, QPixmap(":/map_elements_graphics/roadways/two_way_roadway_dotted_line.png"),
                        "Two way roadway with dotted line", EMovementPlane::vertical);
}

CStationaryMapElement *CRoadway::create_two_way_roadway_with_double_continous_line()
{
    return new CRoadway(2, 1, ERoadSurfaceMarking::double_continuous_line, QPixmap(":/map_elements_graphics/roadways/two_way_roadway_double_continuous_line.png"),
                "Two way roadway with double continous line", EMovementPlane::vertical);
}
