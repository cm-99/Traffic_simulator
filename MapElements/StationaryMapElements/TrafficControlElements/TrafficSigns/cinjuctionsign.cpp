#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"

CInjuctionSign::CInjuctionSign(QPixmap sign_pixmap, EInjuctionSigns sign_type, QString description) :
    CTrafficSign(ETrafficSignCategory::injuction_sign, description)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}

CStationaryMapElement *CInjuctionSign::create_drive_straight_sign()
{
    CInjuctionSign *drive_straight_sign =
        new CInjuctionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/injuction_signs/drive_straight_sign.png")),
                           EInjuctionSigns::drive_straight, "Drive straight");
    return drive_straight_sign;
}

CStationaryMapElement *CInjuctionSign::create_turn_left_sign()
{
    CInjuctionSign *turn_left_sign =
        new CInjuctionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/injuction_signs/turn_left_sign.png")),
                           EInjuctionSigns::turn_left, "Turn left");
    return turn_left_sign;
}

CStationaryMapElement *CInjuctionSign::create_turn_right_sign()
{
    CInjuctionSign *turn_right_sign =
        new CInjuctionSign(QPixmap(QPixmap(":/map_elements_graphics/signs/injuction_signs/turn_right_sign.png")),
                           EInjuctionSigns::turn_right, "Turn right");
    return turn_right_sign;
}
