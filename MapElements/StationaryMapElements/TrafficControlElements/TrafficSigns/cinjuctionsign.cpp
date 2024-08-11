#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"

CInjuctionSign::CInjuctionSign(QPixmap sign_pixmap, SInjuctionSignType sign_type, QString description, QString pixmap_path) :
    CTrafficSign(ETrafficSignCategory::injuction_sign, description, pixmap_path),
    m_sign_type(sign_type)
{
    setPixmap(sign_pixmap);
}

CStationaryMapElement *CInjuctionSign::create_drive_straight_sign()
{
    CInjuctionSign *drive_straight_sign =
        new CInjuctionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/injuction_signs/drive_straight_sign.png")),
                           EInjuctionSigns::drive_straight, "Drive straight", ":graphics/map_elements_graphics/signs/injuction_signs/drive_straight_sign.png");
    return drive_straight_sign;
}

CStationaryMapElement *CInjuctionSign::create_turn_left_sign()
{
    CInjuctionSign *turn_left_sign =
        new CInjuctionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/injuction_signs/turn_left_sign.png")),
                           EInjuctionSigns::turn_left, "Turn left", ":graphics/map_elements_graphics/signs/injuction_signs/turn_left_sign.png");
    return turn_left_sign;
}

CStationaryMapElement *CInjuctionSign::create_turn_right_sign()
{
    CInjuctionSign *turn_right_sign =
        new CInjuctionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/injuction_signs/turn_right_sign.png")),
                           EInjuctionSigns::turn_right, "Turn right", ":graphics/map_elements_graphics/signs/injuction_signs/turn_right_sign.png");
    return turn_right_sign;
}

QString CInjuctionSign::serialize_as_string()
{
    return get_pixmap_path() + "," + m_sign_type.type_number_as_string() + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CInjuctionSign::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::traffic_control_element) + "_" +
           QString::number(ETrafficControlElementType::traffic_sign) + "_" +
           QString::number(ETrafficSignCategory::injuction_sign);
}

CStationaryMapElement *CInjuctionSign::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 4){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid injuction sign serialization");
        return nullptr;
    }
    CStationaryMapElement *item = new CInjuctionSign(QPixmap(item_attributes_list[0]),
                                                     static_cast<EInjuctionSigns>(item_attributes_list[1].toInt()), "",
                                                     item_attributes_list[0]);

    QPointF pos(item_attributes_list[2].toInt(), item_attributes_list[3].toInt());
    item->setPos(pos);

    return item;
}
