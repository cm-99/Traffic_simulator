#include "cfiller.h"

CFiller::CFiller() :
    CStationaryMapElement(EStationaryMapElementType::filler, "Filler", true, ":/map_elements_graphics/filler.png")
{
    setPixmap(QPixmap(":/map_elements_graphics/filler.png"));
    this -> setZValue(-2);
}
