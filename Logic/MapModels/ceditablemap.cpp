#include "ceditablemap.h"
#include "MapElements/StationaryMapElements/cfiller.h"

CEditableMap::CEditableMap(int scene_width, int scene_height):
    CReadOnlyMap(scene_width, scene_height)
{}

void CEditableMap::resize_map(int scene_width, int scene_height)
{
    if(scene_width == this->width()){
        return;
    }

    erase_guide_grid();

    if(scene_width < this->width()){
        shrink_map(scene_width, scene_height);
    }
    else{
        enlarge_map(scene_width, scene_height);
    }
}

void CEditableMap::fill_map()
{
    QSize default_map_element_size = CStationaryMapElement::get_default_map_element_size();
    int map_fillers_rows = this->height()/default_map_element_size.height();
    int map_fillers_columns = this->width()/default_map_element_size.width();

    for(int i = 0; i < map_fillers_rows; i++){
        for(int j = 0; j < map_fillers_columns; j++){
            auto *filler = new CFiller();
            this->add_stationary_map_element(filler, QPointF(j * default_map_element_size.width(), i * default_map_element_size.height()));
        }
    }
}

void CEditableMap::add_guide_grid()
{
    QSize default_element_size = CStationaryMapElement::get_default_map_element_size();
    int default_element_width = default_element_size.width();
    int default_element_height = default_element_size.height();
    int map_model_width = this->width();
    int map_model_height = this->height();
    int horizontal_guide_lines_number = map_model_height/default_element_height;
    int vertical_guide_lines_number = map_model_width/default_element_width;

    for(int i = 0; i <= horizontal_guide_lines_number; i++){
        int line_y_pos = i * default_element_height;
        this->addLine(0, line_y_pos, map_model_width, line_y_pos);
    }


    for(int i = 0; i <= vertical_guide_lines_number; i++){
        int line_x_pos = i * default_element_width;
        this->addLine(line_x_pos, 0, line_x_pos, map_model_height);
    }
}

void CEditableMap::add_map_boundaries()
{
    this->addLine(0, 1, this->width(), 1);
    this->addLine(0, this->height() - 1, this->width(), this->height() - 1);

    this->addLine(0, 0, 0, this->height());
    this->addLine(this->width(), 0, this->width(), this->height());
}

void CEditableMap::clear_stationary_map_elements()
{
    for(auto element : *m_stationary_map_elements){
        removeItem(element);
        delete element;
    }

    m_stationary_map_elements->clear();
    m_traffic_lights->clear();
}

void CEditableMap::add_road_user(CRoadUser *new_road_user, QPointF pos)
{
    new_road_user->setPos(pos);
    m_road_users->append(new_road_user);
    addItem(new_road_user);
}

void CEditableMap::add_stationary_map_element(CStationaryMapElement *new_stationary_map_element, QPointF pos)
{
    new_stationary_map_element->setPos(pos);
    //Omit fillers, waste of space
    if(new_stationary_map_element->get_map_element_type() != EStationaryMapElementType::filler){
        m_stationary_map_elements->append(new_stationary_map_element);
    }
    addItem(new_stationary_map_element);

    auto *traffic_light = dynamic_cast<CTrafficLight*>(new_stationary_map_element);
    if(traffic_light != nullptr){
        m_traffic_lights->append(traffic_light);
    }
}

void CEditableMap::erase_road_user(CRoadUser *road_user_to_remove)
{
    m_road_users->remove(m_road_users->indexOf(road_user_to_remove));
    removeItem(road_user_to_remove);
    delete road_user_to_remove;
}

void CEditableMap::erase_road_users()
{
    for(auto road_user : *m_road_users){
        erase_road_user(road_user);
    }
}

void CEditableMap::erase_item(QGraphicsItem *item)
{
    removeItem(item);
    delete item;
}

void CEditableMap::erase_stationary_map_element(CStationaryMapElement *item)
{
    m_stationary_map_elements->removeAt(m_stationary_map_elements->indexOf(item));

    auto *traffic_light = dynamic_cast<CTrafficLight*>(item);
    if(traffic_light != nullptr){
        traffic_light->break_all_lights_linking();
        m_traffic_lights->remove(m_traffic_lights->indexOf(traffic_light));
    }

    removeItem(item);
    delete item;
}

void CEditableMap::erase_guide_grid()
{
    auto map_items = this->items();

    for(auto item : map_items){
        auto line = dynamic_cast<QGraphicsLineItem*>(item);
        if(line){
            removeItem(line);
            delete line;
        }
    }
}

void CEditableMap::erase_map_boundaries()
{
    auto items_at_line_pos = this->items(QPoint(0, 1));
    for(auto item : items_at_line_pos){
        if(dynamic_cast<QGraphicsLineItem*>(item)){
            this->removeItem(item);
            delete item;
        }
    }

    items_at_line_pos = this->items(QPoint(0, this->height() - 1));
    for(auto item : items_at_line_pos){
        if(dynamic_cast<QGraphicsLineItem*>(item)){
            this->removeItem(item);
            delete item;
        }
    }

    items_at_line_pos = this->items(QPoint(0, 0));
    for(auto item : items_at_line_pos){
        if(dynamic_cast<QGraphicsLineItem*>(item)){
            this->removeItem(item);
            delete item;
        }
    }

    items_at_line_pos = this->items(QPoint(this->width(), 0));
    for(auto item : items_at_line_pos){
        if(dynamic_cast<QGraphicsLineItem*>(item)){
            this->removeItem(item);
            delete item;
        }
    }
}

void CEditableMap::enlarge_map(int scene_width, int scene_height)
{
    QSize default_map_element_size = CStationaryMapElement::get_default_map_element_size();
    int previous_map_fillers_rows = this->height()/default_map_element_size.height();
    int previous_map_fillers_columns = this->width()/default_map_element_size.width();

    setSceneRect(0, 0, scene_width, scene_height);
    int map_fillers_rows = this->height()/default_map_element_size.height();
    int map_fillers_columns = this->width()/default_map_element_size.width();

    for(int i = previous_map_fillers_rows; i < map_fillers_rows; i++){
        for(int j = 0; j < map_fillers_columns; j++){
            auto *filler = new CFiller();
            this->add_stationary_map_element(filler, QPointF(j * default_map_element_size.width(), i * default_map_element_size.height()));
        }
    }

    for(int i = 0; i < map_fillers_rows; i++){
        for(int j = previous_map_fillers_columns; j < map_fillers_columns; j++){
            auto *filler = new CFiller();
            this->add_stationary_map_element(filler, QPointF(j * default_map_element_size.width(), i * default_map_element_size.height()));
        }
    }

    add_guide_grid();
    add_map_boundaries();
}

void CEditableMap::shrink_map(int scene_width, int scene_height)
{
    auto map_items = this->items();

    for(auto item : map_items){
        if(item->pos().x() >= scene_width || item->pos().y() >= scene_height){

            auto stationary_element = dynamic_cast<CStationaryMapElement*>(item);

            if(stationary_element && stationary_element->get_map_element_type() != EStationaryMapElementType::filler){
                erase_stationary_map_element(stationary_element);
            }
            else{
                erase_item(item);
            }
        }
    }

    setSceneRect(0, 0, scene_width, scene_height);
    add_guide_grid();
    add_map_boundaries();
}
