#include "cpedestriancrossing.h"

CPedestrianCrossing::CPedestrianCrossing(QPixmap pedestrian_crossing_pixmap) : CRoadElement(ERoadElementType::pedestrian_crossing)
{
    setPixmap(pedestrian_crossing_pixmap);
    this -> setZValue(-1);
}
