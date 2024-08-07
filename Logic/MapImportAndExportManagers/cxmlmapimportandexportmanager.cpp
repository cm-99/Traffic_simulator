#include "cxmlmapimportandexportmanager.h"
#include "MapElements/StationaryMapElements/RoadElements/ccrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/croadway.h"
#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"
#include "MapElements/StationaryMapElements/RoadElements/cturn.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"
#include "MapElements/StationaryMapElements/croadelement.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>

CXMLMapImportAndExportManager::CXMLMapImportAndExportManager() {}

CEditableMap *CXMLMapImportAndExportManager::load_map(QString map_file_path)
{
    QFile map_file(map_file_path);
    if (!(map_file.open(QIODevice::ReadWrite)))
    {
        map_file.close();
        return nullptr;
    }

    QXmlStreamReader reader_XML(&map_file);
    CEditableMap* map;

    reader_XML.readNextStartElement();
    while(reader_XML.name().toString() != "Map_1.0" && reader_XML.error() == 0)
    {
        reader_XML.readNextStartElement();
    }
    reader_XML.readNextStartElement();

    if(reader_XML.name().toString() == "Map_size")
    {
        QString map_size = reader_XML.readElementText();
        QStringList map_size_splitted = map_size.split(",");
        map = new CEditableMap(map_size_splitted[0].toInt(), map_size_splitted[1].toInt());
        reader_XML.readNextStartElement();
    }
    else {
        map_file.close();
        return nullptr;}

    int elements_count = 0;
    if(reader_XML.name().toString() == "Elements_count")
    {
        elements_count = reader_XML.readElementText().toInt();
        reader_XML.readNextStartElement();
    }
    else {
        map_file.close();
        delete map;
        return nullptr;}

    if(reader_XML.name().toString() == "Map_elements")
    {
        for(int i = 0; i < elements_count; i++){
            reader_XML.readNextStartElement();
            QString serialized_element_type = reader_XML.name().toString();
            QString serialized_item = reader_XML.readElementText();
            QStringList element_types_list = serialized_element_type.split("_");
            element_types_list.removeFirst(); //Dropping the character needed for proper XML file
            CStationaryMapElement *deserialized_element = nullptr;
            auto map_element_type = element_types_list[0].toInt();

            if(map_element_type == EStationaryMapElementType::road_element){
                auto road_element_type = element_types_list[1].toInt();
                if(road_element_type == ERoadElementType::pavement){
                    //TODO: virtual cannot be static, without static I need object (cannot be nullptr), how to do it better?
                    deserialized_element = CPavement::deserialize_from_string(serialized_item);
                }
                else if(road_element_type == ERoadElementType::pedestrian_crossing){
                    deserialized_element = CPedestrianCrossing::deserialize_from_string(serialized_item);
                }
                else if(road_element_type == ERoadElementType::roadway_element){
                    auto roadway_element_type = element_types_list[2].toInt();
                    switch (roadway_element_type){
                    case ERoadwayElementType::roadway:
                        deserialized_element = CRoadway::deserialize_from_string(serialized_item);
                        break;
                    case ERoadwayElementType::crossing:
                        deserialized_element = CCrossing::deserialize_from_string(serialized_item);
                        break;
                    case ERoadwayElementType::turn:
                        deserialized_element = CTurn::deserialize_from_string(serialized_item);
                        break;
                    }
                }
            }
            else{//It's traffic control element
                auto traffic_control_element_type = element_types_list[1].toInt();
                if(traffic_control_element_type == ETrafficControlElementType::traffic_lights){
                    deserialized_element = CTrafficLight::deserialize_from_string(serialized_item);
                }
                else{ //traffic sign
                    auto traffic_sign_type = element_types_list[2].toInt();
                    switch (traffic_sign_type){
                    case ETrafficSignCategory::information_sign:
                        deserialized_element = CInformationSign::deserialize_from_string(serialized_item);
                        break;
                    case ETrafficSignCategory::injuction_sign:
                        deserialized_element = CInjuctionSign::deserialize_from_string(serialized_item);
                        break;
                    case ETrafficSignCategory::prohibition_sign:
                        deserialized_element = CProhibitionSign::deserialize_from_string(serialized_item);
                        break;
                    case ETrafficSignCategory::warning_sign:
                        deserialized_element = CWarningSign::deserialize_from_string(serialized_item);
                        break;
                    }
                }
            }
            map->add_stationary_map_element(deserialized_element, deserialized_element->pos());
        }
    }
    else {
        map_file.close();
        delete map;
        return nullptr;}

    return map;
}

void CXMLMapImportAndExportManager::save_map(const CReadOnlyMap *map_to_save, QString map_file_path)
{
    QFile map_file(map_file_path);
    if (!(map_file.open(QIODevice::ReadWrite)))
    {
        map_file.close();
        QMessageBox::warning(nullptr, "Saving unsuccessful", "Could not open selected file.");
        return;
    }

    auto map_items(*map_to_save->get_stationary_map_elements());
    QString map_size = QString::number(map_to_save->get_scene_rect().width()) + "," + QString::number(map_to_save->get_scene_rect().height());

    QXmlStreamWriter writer_XML(&map_file);
    writer_XML.writeStartDocument();
    writer_XML.setAutoFormatting(true);
    writer_XML.setAutoFormattingIndent(4);
    writer_XML.writeStartElement("Map_1.0");
    writer_XML.writeTextElement("Map_size", map_size);
    writer_XML.writeTextElement("Elements_count", QString::number(map_items.size()));
    writer_XML.writeStartElement("Map_elements");
    for(auto item : map_items){
        writer_XML.writeTextElement(item->serialize_type_as_string(), item->serialize_as_string());
    }
    writer_XML.writeEndElement(); //Map_elements
    writer_XML.writeEndElement(); //Map_1.0
    writer_XML.writeEndDocument();
    map_file.close();
}
