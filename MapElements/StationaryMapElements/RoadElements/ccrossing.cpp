#include "ccrossing.h"

CCrossing::CCrossing(QPixmap crossing_pixmap) :
    CRoadElement(ERoadElementType::crossing)
{
    setPixmap(crossing_pixmap);
}
