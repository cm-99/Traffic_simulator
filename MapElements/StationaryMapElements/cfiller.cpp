#include "cfiller.h"

CFiller::CFiller() :
    CStationaryMapElement(EStationaryMapElementType::filler, "Filler", true, ":graphics/map_elements_graphics/filler.png")
{
    setPixmap(QPixmap(":graphics/map_elements_graphics/filler.png"));
    this -> setZValue(-2);
}
