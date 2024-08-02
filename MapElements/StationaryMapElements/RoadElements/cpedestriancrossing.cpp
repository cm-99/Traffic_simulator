#include "cpedestriancrossing.h"

CStationaryMapElement *CPedestrianCrossing::create_pedestrian_crossing()
{
    return new CPedestrianCrossing(QPixmap(":/map_elements_graphics/pedestrian_crossing.png"), "Pedestrian crossing", EMovementPlane::horizontal);
}

CPedestrianCrossing::CPedestrianCrossing(QPixmap pedestrian_crossing_pixmap, QString description, EMovementPlane movement_plane) :
    CRoadElement(ERoadElementType::pedestrian_crossing, movement_plane, EPermittedRoadUsers::all, description)
{
    setPixmap(pedestrian_crossing_pixmap);
}
