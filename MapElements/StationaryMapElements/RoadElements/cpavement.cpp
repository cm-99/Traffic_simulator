#include "cpavement.h"

CPavement::CPavement(QPixmap pavement_pixmap) : CRoadElement(ERoadElementType::pavement)
{
    setPixmap(pavement_pixmap);
    this -> setZValue(-1);
}
