#include "cturn.h"

CTurn::CTurn(int carriageways_number, int lanes_number, QPixmap turn_pixmap, QString description, QString pixmap_path,
             EHorizontalMoveDirection horizontal_direction, EVerticalMoveDirection vertical_direction) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::turn, EMovementPlane::horizontal, EPermittedRoadUsers::cars, description, pixmap_path)
{
    setPixmap(turn_pixmap);
    m_horizontal_direction = horizontal_direction;
    m_vertical_direction = vertical_direction;
}

CStationaryMapElement *CTurn::create_double_carriageway_car_turn_with_single_lane()
{
    return new CTurn(2, 1, QPixmap(":/map_elements_graphics/turns/double_carriageway_single_lane_turn.png"),
    "Double carriageway turn with single lane", ":/map_elements_graphics/turns/double_carriageway_single_lane_turn.png");
}

CStationaryMapElement *CTurn::create_single_carriageway_car_turn_with_single_lane()
{
    return new CTurn(1, 1, QPixmap(":/map_elements_graphics/turns/single_carriageway_single_lane_turn.png"),
                     "Single carriageway turn with single lane", ":/map_elements_graphics/turns/single_carriageway_single_lane_turn.png");
}

QString CTurn::serialize_as_string()
{
    return get_pixmap_path() + "," + QString::number(m_horizontal_direction) + "," + QString::number(m_vertical_direction) + "," +
           QString::number(get_carriageways_number()) + "," + QString::number(get_lanes_number()) + "," +
           QString::number((int)rotation()) + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CTurn::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::road_element) + "_" +
           QString::number(ERoadElementType::roadway_element) + "_" +
           QString::number(ERoadwayElementType::turn);
}

CStationaryMapElement *CTurn::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 8){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid turn serialization");
        return nullptr;
    }

    auto item = new CTurn(item_attributes_list[3].toInt(), item_attributes_list[4].toInt(), QPixmap(item_attributes_list[0]), "", item_attributes_list[0],
                          static_cast<EHorizontalMoveDirection>(item_attributes_list[1].toInt()),
                          static_cast<EVerticalMoveDirection>(item_attributes_list[2].toInt()));

    QPointF pos(item_attributes_list[6].toInt(), item_attributes_list[7].toInt());
    QPointF center = item->boundingRect().center();
    item->setTransformOriginPoint(center);
    item->setRotation(item_attributes_list[5].toInt());
    item->setPos(pos);

    return item;
}
