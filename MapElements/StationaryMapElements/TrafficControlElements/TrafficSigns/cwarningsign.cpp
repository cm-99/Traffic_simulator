#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"

CWarningSign::CWarningSign(QPixmap sign_pixmap, SWarningSignType sign_type, QString description, QString pixmap_path) :
    CTrafficSign(ETrafficSignCategory::warning_sign, description, pixmap_path),
    m_sign_type(sign_type)
{
    setPixmap(sign_pixmap);
    m_sign_type = sign_type;
}

CStationaryMapElement *CWarningSign::create_give_way_sign()
{
    CWarningSign *give_way_sign =
        new CWarningSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/warning_signs/give_way_sign.png")),
                         EWarningSigns::give_way, "Give way sign", ":graphics/map_elements_graphics/signs/warning_signs/give_way_sign.png");
    return give_way_sign;
}

CStationaryMapElement *CWarningSign::create_equivalent_intersection_sign()
{
    CWarningSign *give_way_sign =
        new CWarningSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/warning_signs/equivalent_intersection_sign.png")),
                         EWarningSigns::equivalent_intersection, "Equivalent intersection sign", ":graphics/map_elements_graphics/signs/warning_signs/equivalent_intersection_sign.png");
    return give_way_sign;
}

QString CWarningSign::serialize_as_string()
{
    return get_pixmap_path() + "," + m_sign_type.type_number_as_string() + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CWarningSign::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::traffic_control_element) + "_" +
           QString::number(ETrafficControlElementType::traffic_sign) + "_" +
           QString::number(ETrafficSignCategory::warning_sign);
}

CStationaryMapElement *CWarningSign::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 4){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid warning sign serialization");
        return nullptr;
    }
    CStationaryMapElement *item = new CWarningSign(QPixmap(item_attributes_list[0]),
                                                   static_cast<EWarningSigns>(item_attributes_list[1].toInt()), "",
                                                   item_attributes_list[0]);

    QPointF pos(item_attributes_list[2].toInt(), item_attributes_list[3].toInt());
    item->setPos(pos);

    return item;
}
