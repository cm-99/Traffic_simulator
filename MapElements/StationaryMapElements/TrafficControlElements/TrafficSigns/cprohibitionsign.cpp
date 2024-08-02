#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"

CProhibitionSign::CProhibitionSign(QPixmap sign_pixmap, EProhibitionSigns sign_type, QString description, int speed_limit) :
    CTrafficSign(ETrafficSignCategory::prohibition_sign, description)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
    m_speed_limit = speed_limit;
}

CStationaryMapElement *CProhibitionSign::create_no_entry_sign()
{
    CProhibitionSign *no_entry_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/no_entry_sign.png")),
                             EProhibitionSigns::no_entry, "No entry", -1);
    return no_entry_sign;
}

CStationaryMapElement *CProhibitionSign::create_stop_sign()
{
    CProhibitionSign *stop_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/stop_sign.png")),
                             EProhibitionSigns::stop, "Stop", -1);
    return stop_sign;
}

CStationaryMapElement *CProhibitionSign::create_left_turn_prohibited_sign()
{
    CProhibitionSign *left_turn_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/left_turn_prohibited_sign.png")),
                             EProhibitionSigns::left_turn_prohibited, "Left turn prohibited", -1);
    return left_turn_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_right_turn_prohibited_sign()
{
    CProhibitionSign *right_turn_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/right_turn_prohibited_sign.png")),
                             EProhibitionSigns::right_turn_prohibited, "Right turn prohibited", -1);
    return right_turn_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_turning_prohibited_sign()
{
    CProhibitionSign *turning_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/turning_prohibited_sign.png")),
                             EProhibitionSigns::turning_prohibited, "Turning prohibited", -1);
    return turning_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_speed_limit_30_sign()
{
    CProhibitionSign *speed_limit_30_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/speed_limit_30_sign.png")),
                             EProhibitionSigns::speed_limit, "Speed limit 30", 30);
    return speed_limit_30_sign;
}

CStationaryMapElement *CProhibitionSign::create_speed_limit_50_sign()
{
    CProhibitionSign *speed_limit_50_sign =
        new CProhibitionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/prohibition_signs/speed_limit_50_sign.png")),
                             EProhibitionSigns::speed_limit, "Speed limit 50", 50);
    return speed_limit_50_sign;
}
