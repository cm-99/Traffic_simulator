#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"

CInjuctionSign::CInjuctionSign(QPixmap sign_pixmap, EInjuctionSigns sign_type) :
    CTrafficSign(ETrafficSignCategory::injuction_sign)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}
