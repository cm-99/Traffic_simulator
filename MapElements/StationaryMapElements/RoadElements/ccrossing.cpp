#include "ccrossing.h"

CCrossing::CCrossing(int carriageways_number, int lanes_number, QPixmap crossing_pixmap, EPermittedRoadUsers permitted_road_users, QString description, EMovementPlane movement_plane) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::crossing, movement_plane, permitted_road_users, description)
{
    setPixmap(crossing_pixmap);
}

CStationaryMapElement *CCrossing::create_single_carriageway_car_crossing()
{
    return new CCrossing(1, 1, QPixmap(":/map_elements_graphics/crossings/single_carriageway_crossing.png"), EPermittedRoadUsers::cars,
                "Single carriageway crossing");
}

CStationaryMapElement *CCrossing::create_double_carriageway_car_crossing()
{
    return new CCrossing(2, 1, QPixmap(":/map_elements_graphics/crossings/double_carriageway_crossing.png"), EPermittedRoadUsers::cars,
                        "Double carriageway crossing");
}

