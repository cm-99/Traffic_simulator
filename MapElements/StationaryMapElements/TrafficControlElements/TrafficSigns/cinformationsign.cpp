#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"

CInformationSign::CInformationSign(QPixmap sign_pixmap, SInformationSignType sign_type, QString description, QString pixmap_path) :
    CTrafficSign(ETrafficSignCategory::information_sign, description, pixmap_path),
    m_sign_type(sign_type)
{
    setPixmap(sign_pixmap);
}

CStationaryMapElement *CInformationSign::create_priority_road_sign()
{
    CInformationSign *priority_road_sign =
        new CInformationSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/information_signs/priority_road_sign.png")),
                           EInformationSigns::priority_road, "Priority road", ":graphics/map_elements_graphics/signs/information_signs/priority_road_sign.png");
    return priority_road_sign;
}

CStationaryMapElement *CInformationSign::create_pedestrian_crossing_sign()
{
    CInformationSign *pedestrian_crossing_sign =
        new CInformationSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/information_signs/pedestrian_crossing_sign.png")),
                           EInformationSigns::pedestrian_crossing_sign, "Pedestrian crossing", ":graphics/map_elements_graphics/signs/information_signs/pedestrian_crossing_sign.png");
    return pedestrian_crossing_sign;
}

CStationaryMapElement *CInformationSign::create_one_wady_road_sign()
{
    CInformationSign *one_wady_road_sign =
        new CInformationSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/information_signs/one_way_road_sign.png")),
                            EInformationSigns::one_way_road, "One way road", ":graphics/map_elements_graphics/signs/information_signs/one_way_road_sign.png");
    return one_wady_road_sign;
}

CStationaryMapElement *CInformationSign::create_built_up_area_sign()
{
    CInformationSign *built_up_area_sign =
        new CInformationSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/information_signs/built_up_area_sign.png")),
                             EInformationSigns::built_up_area_sign, "Built-up area", ":graphics/map_elements_graphics/signs/information_signs/built_up_area_sign.png");
    return built_up_area_sign;
}

CStationaryMapElement *CInformationSign::create_non_built_up_area_sign()
{
    CInformationSign *non_built_up_area_sign =
        new CInformationSign(QPixmap(QPixmap(":graphics/map_elements_graphics/signs/information_signs/non_built_up_area_sign.png")),
                             EInformationSigns::non_built_up_area_sign, "Non-built-up area", ":graphics/map_elements_graphics/signs/information_signs/non_built_up_area_sign.png");
    return non_built_up_area_sign;
}

QString CInformationSign::serialize_as_string()
{
    return get_pixmap_path() + "," + m_sign_type.type_number_as_string() + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CInformationSign::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::traffic_control_element) + "_" +
           QString::number(ETrafficControlElementType::traffic_sign) + "_" +
           QString::number(ETrafficSignCategory::information_sign);
}

CStationaryMapElement *CInformationSign::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 4){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid information sign serialization");
        return nullptr;
    }
    CStationaryMapElement *item = new CInformationSign(QPixmap(item_attributes_list[0]),
                                                     static_cast<EInformationSigns>(item_attributes_list[1].toInt()), "",
                                                     item_attributes_list[0]);

    QPointF pos(item_attributes_list[2].toInt(), item_attributes_list[3].toInt());
    item->setPos(pos);

    return item;
}
