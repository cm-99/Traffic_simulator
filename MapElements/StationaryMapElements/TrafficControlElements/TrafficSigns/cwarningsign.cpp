#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"

CWarningSign::CWarningSign(QPixmap sign_pixmap, EWarningSigns sign_type) :
    CTrafficSign(ETrafficSignCategory::warning_sign)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}
