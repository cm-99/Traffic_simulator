#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"

CProhibitionSign::CProhibitionSign(QPixmap sign_pixmap, EProhibitionSigns sign_type) :
    CTrafficSign(ETrafficSignCategory::prohibition_sign)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}
