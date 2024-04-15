#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"

CInformationSign::CInformationSign(QPixmap sign_pixmap, EInformationSigns sign_type) :
    CTrafficSign(ETrafficSignCategory::information_sign)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}
