#ifndef CROADWAY_H
#define CROADWAY_H

#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"

/**
 * @brief The ERoadSurfaceMarking enum stores roadway markings to enable implementation of lane changing, turning and overtaking
 */
enum ERoadSurfaceMarking {neither, single_dotted_line, single_continuous, double_continuous_line};

/**
 * @brief The CRoadway class can be used to represent different types of roadways depending on initialization arguments.
 */
class CRoadway : public CRoadwayElement
{
public:
    //TODO: create lines as graphical masks and switch to builder pattern e.g. create_two_way_roadway().with_dotted_line()
    static CStationaryMapElement *create_one_way_roadway();
    static CStationaryMapElement *create_two_way_roadway_with_dotted_line();
    static CStationaryMapElement *create_two_way_roadway_with_double_continous_line();

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

    ERoadSurfaceMarking get_roadway_marking() const {return m_roadway_surface_marking;}

private:
    CRoadway(int carriageways_number, int lanes_number, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap,
             QString description, EMovementPlane movement_plane, QString pixmap_path);
    ERoadSurfaceMarking m_roadway_surface_marking;
};

#endif // CROADWAY_H
