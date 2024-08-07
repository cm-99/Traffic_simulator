#include "cpavement.h"

CStationaryMapElement *CPavement::create_pavement()
{
    return new CPavement(QPixmap(":/map_elements_graphics/crossings/pavement_crossing.png"), "Pavement", EMovementPlane::any, ":/map_elements_graphics/crossings/pavement_crossing.png");
}

CPavement::CPavement(QPixmap pavement_pixmap, QString description, EMovementPlane movement_plane, QString pixmap_path) :
    CRoadElement(ERoadElementType::pavement, movement_plane, EPermittedRoadUsers::pedestrians, description, pixmap_path)
{
    setPixmap(pavement_pixmap);
}

QString CPavement::serialize_as_string()
{
    return get_pixmap_path() + "," + QString::number(get_movement_plane()) + "," +
           QString::number((int)rotation()) + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CPavement::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::road_element) + "_" +
           QString::number(ERoadElementType::pavement);
}

CStationaryMapElement *CPavement::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 5){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid pavement serialization");
        return nullptr;
    }

    auto item = new CPavement(QPixmap(item_attributes_list[0]), "",
                                        static_cast<EMovementPlane>(item_attributes_list[1].toInt()), item_attributes_list[0]);

    QPointF pos(item_attributes_list[3].toInt(), item_attributes_list[4].toInt());
    QPointF center = item->boundingRect().center();
    item->setTransformOriginPoint(center);
    item->setRotation(item_attributes_list[2].toInt());
    item->setPos(pos);

    return item;
}
