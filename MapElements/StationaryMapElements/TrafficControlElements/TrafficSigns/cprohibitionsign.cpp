#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"

CProhibitionSign::CProhibitionSign(QPixmap sign_pixmap, SProhibitionSignType sign_type, QString description, int speed_limit, QString pixmap_path) :
    CTrafficSign(ETrafficSignCategory::prohibition_sign, description, pixmap_path),
    m_sign_type(sign_type)
{
    setPixmap(sign_pixmap);
    m_speed_limit = speed_limit;
}

CStationaryMapElement *CProhibitionSign::create_no_entry_sign()
{
    CProhibitionSign *no_entry_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/no_entry_sign.png")),
                             EProhibitionSigns::no_entry, "No entry", -1, ":graphics/map_elements_graphics/signs/prohibition_signs/no_entry_sign.png");
    return no_entry_sign;
}

CStationaryMapElement *CProhibitionSign::create_stop_sign()
{
    CProhibitionSign *stop_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/stop_sign.png")),
                             EProhibitionSigns::stop, "Stop", -1, ":graphics/map_elements_graphics/signs/prohibition_signs/stop_sign.png");
    return stop_sign;
}

CStationaryMapElement *CProhibitionSign::create_left_turn_prohibited_sign()
{
    CProhibitionSign *left_turn_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/left_turn_prohibited_sign.png")),
                             EProhibitionSigns::left_turn_prohibited, "Left turn prohibited", -1, ":graphics/map_elements_graphics/signs/prohibition_signs/left_turn_prohibited_sign.png");
    return left_turn_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_right_turn_prohibited_sign()
{
    CProhibitionSign *right_turn_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/right_turn_prohibited_sign.png")),
                             EProhibitionSigns::right_turn_prohibited, "Right turn prohibited", -1, ":graphics/map_elements_graphics/signs/prohibition_signs/right_turn_prohibited_sign.png");
    return right_turn_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_turning_prohibited_sign()
{
    CProhibitionSign *turning_prohibited_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/turning_prohibited_sign.png")),
                             EProhibitionSigns::turning_prohibited, "Turning prohibited", -1, ":graphics/map_elements_graphics/signs/prohibition_signs/turning_prohibited_sign.png");
    return turning_prohibited_sign;
}

CStationaryMapElement *CProhibitionSign::create_speed_limit_30_sign()
{
    CProhibitionSign *speed_limit_30_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/speed_limit_30_sign.png")),
                             EProhibitionSigns::speed_limit, "Speed limit 30", 30, ":graphics/map_elements_graphics/signs/prohibition_signs/speed_limit_30_sign.png");
    return speed_limit_30_sign;
}

CStationaryMapElement *CProhibitionSign::create_speed_limit_50_sign()
{
    CProhibitionSign *speed_limit_50_sign =
        new CProhibitionSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/prohibition_signs/speed_limit_50_sign.png")),
                             EProhibitionSigns::speed_limit, "Speed limit 50", 50, ":graphics/map_elements_graphics/signs/prohibition_signs/speed_limit_50_sign.png");
    return speed_limit_50_sign;
}

QString CProhibitionSign::serialize_as_string()
{
    return get_pixmap_path() + "," + m_sign_type.type_number_as_string() + "," + QString::number(m_speed_limit) + "," +
           QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CProhibitionSign::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::traffic_control_element) + "_" +
           QString::number(ETrafficControlElementType::traffic_sign) + "_" +
           QString::number(ETrafficSignCategory::prohibition_sign);
}

int CProhibitionSign::get_traffic_sign_type_as_int()
{
    return m_sign_type.m_type;
}


CStationaryMapElement *CProhibitionSign::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 5){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid prohibition sign serialization");
        return nullptr;
    }
    CStationaryMapElement *item = new CProhibitionSign(QPixmap(item_attributes_list[0]),
                                                    static_cast<EProhibitionSigns>(item_attributes_list[1].toInt()), "",
                                                    item_attributes_list[2].toInt(),
                                                    item_attributes_list[0]);

    QPointF pos(item_attributes_list[3].toInt(), item_attributes_list[4].toInt());
    item->setPos(pos);

    return item;
}
