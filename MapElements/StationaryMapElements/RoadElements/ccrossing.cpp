#include "ccrossing.h"

CCrossing::CCrossing(int carriageways_number, int lanes_number, QPixmap crossing_pixmap, EPermittedRoadUsers permitted_road_users, QString description, QString pixmap_path, EMovementPlane movement_plane) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::crossing, movement_plane, permitted_road_users, description, pixmap_path)
{
    setPixmap(crossing_pixmap);
}

CStationaryMapElement *CCrossing::create_single_carriageway_car_crossing()
{
    return new CCrossing(1, 1, QPixmap(":/map_elements_graphics/crossings/single_carriageway_crossing.png"), EPermittedRoadUsers::cars,
                "Single carriageway crossing", ":/map_elements_graphics/crossings/single_carriageway_crossing.png");
}

CStationaryMapElement *CCrossing::create_double_carriageway_car_crossing()
{
    return new CCrossing(2, 1, QPixmap(":/map_elements_graphics/crossings/double_carriageway_crossing.png"), EPermittedRoadUsers::cars,
                        "Double carriageway crossing", ":/map_elements_graphics/crossings/double_carriageway_crossing.png");
}

QString CCrossing::serialize_as_string()
{
    return get_pixmap_path() + "," + QString::number(get_carriageways_number()) + "," + QString::number(get_lanes_number()) + "," +
           QString::number(get_movement_plane()) + "," + QString::number((int)rotation()) + "," + QString::number(pos().x()) + "," +
           QString::number(pos().y());
}

QString CCrossing::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::road_element) + "_" +
           QString::number(ERoadElementType::roadway_element) + "_" +
           QString::number(ERoadwayElementType::crossing);
}

CStationaryMapElement *CCrossing::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 7){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid crossing serialization");
        return nullptr;
    }

    auto item = new CCrossing(item_attributes_list[1].toInt(), item_attributes_list[2].toInt(),
                             QPixmap(item_attributes_list[0]), EPermittedRoadUsers::cars, "", item_attributes_list[0],
                              static_cast<EMovementPlane>(item_attributes_list[3].toInt()));

    QPointF pos(item_attributes_list[5].toInt(), item_attributes_list[6].toInt());
    QPointF center = item->boundingRect().center();
    item->setTransformOriginPoint(center);
    item->setRotation(item_attributes_list[4].toInt());
    item->setPos(pos);

    return item;
}
