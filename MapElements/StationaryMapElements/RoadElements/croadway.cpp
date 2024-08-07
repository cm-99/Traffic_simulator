#include "croadway.h"

CRoadway::CRoadway(int carriageways_number, int lanes_number, ERoadSurfaceMarking roadway_surface_marking, QPixmap roadway_pixmap, QString description, EMovementPlane movement_plane, QString pixmap_path) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::roadway, movement_plane, EPermittedRoadUsers::cars, description, pixmap_path),
    m_roadway_surface_marking(roadway_surface_marking)
{
    setPixmap(roadway_pixmap);
}

CStationaryMapElement *CRoadway::create_one_way_roadway()
{
    return new CRoadway(1, 1, ERoadSurfaceMarking::none, QPixmap(":/map_elements_graphics/roadways/one_way_roadway.png"),
                        "One way roadway", EMovementPlane::vertical, ":/map_elements_graphics/roadways/one_way_roadway.png");
}

CStationaryMapElement *CRoadway::create_two_way_roadway_with_dotted_line()
{
    return new CRoadway(2, 1, ERoadSurfaceMarking::single_dotted_line, QPixmap(":/map_elements_graphics/roadways/two_way_roadway_dotted_line.png"),
                        "Two way roadway with dotted line", EMovementPlane::vertical, ":/map_elements_graphics/roadways/two_way_roadway_dotted_line.png");
}

CStationaryMapElement *CRoadway::create_two_way_roadway_with_double_continous_line()
{
    return new CRoadway(2, 1, ERoadSurfaceMarking::double_continuous_line, QPixmap(":/map_elements_graphics/roadways/two_way_roadway_double_continuous_line.png"),
                "Two way roadway with double continous line", EMovementPlane::vertical, ":/map_elements_graphics/roadways/two_way_roadway_double_continuous_line.png");
}

QString CRoadway::serialize_as_string()
{
    return get_pixmap_path() + "," + QString::number(get_carriageways_number()) + "," + QString::number(get_lanes_number()) + "," +
           QString::number(m_roadway_surface_marking) + "," + QString::number(get_movement_plane()) + "," +
           QString::number((int)rotation()) + "," + QString::number(pos().x()) + "," + QString::number(pos().y());
}

QString CRoadway::serialize_type_as_string()
{
    return QString("A") + "_" + QString::number(EStationaryMapElementType::road_element) + "_" +
           QString::number(ERoadElementType::roadway_element) + "_" +
           QString::number(ERoadwayElementType::roadway);
}

CStationaryMapElement *CRoadway::deserialize_from_string(QString item_serialized_to_string)
{
    QStringList item_attributes_list = item_serialized_to_string.split(",");
    if(item_attributes_list.length() != 8){
        throw std::invalid_argument("Provided argument item_serialized_to_string is not a valid roadway serialization");
        return nullptr;
    }

    auto item = new CRoadway(item_attributes_list[1].toInt(), item_attributes_list[2].toInt(), static_cast<ERoadSurfaceMarking>(item_attributes_list[3].toInt()),
                             QPixmap(item_attributes_list[0]), "", static_cast<EMovementPlane>(item_attributes_list[4].toInt()),
                             item_attributes_list[0]);

    QPointF pos(item_attributes_list[6].toInt(), item_attributes_list[7].toInt());
    QPointF center = item->boundingRect().center();
    item->setTransformOriginPoint(center);
    item->setRotation(item_attributes_list[5].toInt());
    item->setPos(pos);

    return item;
}
