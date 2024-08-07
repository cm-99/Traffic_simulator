#include "cpedestriancrossing.h"

CStationaryMapElement *CPedestrianCrossing::create_pedestrian_crossing()
{
    return new CPedestrianCrossing(QPixmap(":/map_elements_graphics/pedestrian_crossing.png"), "Pedestrian crossing", EMovementPlane::horizontal, ":/map_elements_graphics/pedestrian_crossing.png");
}

CPedestrianCrossing::CPedestrianCrossing(QPixmap pedestrian_crossing_pixmap, QString description, EMovementPlane movement_plane, QString pixmap_path) :
    CRoadElement(ERoadElementType::pedestrian_crossing, movement_plane, EPermittedRoadUsers::all, description, pixmap_path)
{
    setPixmap(pedestrian_crossing_pixmap);
}

QString CPedestrianCrossing::serialize_as_string()
{
    return get_pixmap_path() + "," + QString::number(get_movement_plane()) + "," +
           QString::number((int)rotation()) + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CPedestrianCrossing::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::road_element) + "_" +
           QString::number(ERoadElementType::pedestrian_crossing);
}

CStationaryMapElement *CPedestrianCrossing::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 5){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid pedestrian crossing serialization");
        return nullptr;
    }

    auto item = new CPedestrianCrossing(QPixmap(item_attributes_list[0]), "",
                                        static_cast<EMovementPlane>(item_attributes_list[1].toInt()), item_attributes_list[0]);

    QPointF pos(item_attributes_list[3].toInt(), item_attributes_list[4].toInt());
    QPointF center = item->boundingRect().center();
    item->setTransformOriginPoint(center);
    item->setRotation(item_attributes_list[2].toInt());
    item->setPos(pos);

    return item;
}
