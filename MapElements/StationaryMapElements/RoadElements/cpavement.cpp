#include "cpavement.h"

CStationaryMapElement *CPavement::create_pavement()
{
    return new CPavement(QPixmap(":/map_elements_graphics/crossings/pavement_crossing.png"), "Pavement", EMovementPlane::any);
}

CPavement::CPavement(QPixmap pavement_pixmap, QString description, EMovementPlane movement_plane) :
    CRoadElement(ERoadElementType::pavement, movement_plane, EPermittedRoadUsers::pedestrians, description)
{
    setPixmap(pavement_pixmap);
}
