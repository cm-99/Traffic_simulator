#include "cfiller.h"

CFiller::CFiller() :
    CStationaryMapElement(EStationaryMapElementType::filler, "filler", true)
{
    setPixmap(QPixmap(":/map_elements_graphics/filler.png"));
    this -> setZValue(-2);
}
