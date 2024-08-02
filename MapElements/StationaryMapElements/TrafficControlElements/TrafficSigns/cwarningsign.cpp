#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"

CWarningSign::CWarningSign(QPixmap sign_pixmap, EWarningSigns sign_type, QString description) :
    CTrafficSign(ETrafficSignCategory::warning_sign, description)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}

CStationaryMapElement *CWarningSign::create_give_way_sign()
{
    CWarningSign *give_way_sign =
        new CWarningSign(QPixmap(QPixmap(":/map_elements_graphics/signs/warning_signs/give_way_sign.png")),
                         EWarningSigns::give_way, "Give way");
    return give_way_sign;
}

CStationaryMapElement *CWarningSign::create_equivalent_intersection_sign()
{
    CWarningSign *give_way_sign =
        new CWarningSign(QPixmap(QPixmap(":/map_elements_graphics/signs/warning_signs/equivalent_intersection_sign.png")),
                         EWarningSigns::equivalent_intersection, "Equivalent intersection");
    return give_way_sign;
}
