#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"

CInformationSign::CInformationSign(QPixmap sign_pixmap, EInformationSigns sign_type, QString description) :
    CTrafficSign(ETrafficSignCategory::information_sign, description)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}

CStationaryMapElement *CInformationSign::create_priority_road_sign()
{
    CInformationSign *priority_road_sign =
        new CInformationSign(QPixmap(QPixmap(":/map_elements_graphics/signs/information_signs/priority_road_sign.png")),
                           EInformationSigns::priority_road, "Priority road");
    return priority_road_sign;
}

CStationaryMapElement *CInformationSign::create_pedestrian_crossing_sign()
{
    CInformationSign *pedestrian_crossing_sign =
        new CInformationSign(QPixmap(QPixmap(":/map_elements_graphics/signs/information_signs/pedestrian_crossing_sign.png")),
                           EInformationSigns::pedestrian_crossing, "Pedestrian crossing");
    return pedestrian_crossing_sign;
}

CStationaryMapElement *CInformationSign::create_one_wady_road_sign()
{
    CInformationSign *one_wady_road_sign =
        new CInformationSign(QPixmap(QPixmap(":/map_elements_graphics/signs/information_signs/one_way_road_sign.png")),
                            EInformationSigns::one_way_road, "One way road");
    return one_wady_road_sign;
}

