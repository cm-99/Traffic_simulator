#include "cmapelementsplacementwidget.h"
#include "MapElements/StationaryMapElements/RoadElements/ccrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/cpavement.h"
#include "MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h"
#include "MapElements/StationaryMapElements/RoadElements/croadway.h"
#include "MapElements/StationaryMapElements/RoadElements/cturn.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h"
#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"
#include "cmapcreationpage.h"

CExpandableWidget * CMapElementsPlacementWidget::create_nested_expandable_widget(QFrame *parent_frame, QString title, QLayout& widget_layout)
{
    QFrame *content_widget = new QFrame(parent_frame);
    QVBoxLayout *wrapper_layout = new QVBoxLayout();
    CExpandableWidget *expandable_widget = new CExpandableWidget(title, 300, parent_frame);

    content_widget->setLayout(&widget_layout);
    wrapper_layout->addWidget(content_widget);
    expandable_widget->set_content_layout(*wrapper_layout);

    return expandable_widget;
}

void CMapElementsPlacementWidget::add_button_to_grid_layout(QPushButton *button, QGridLayout *grid_layout)
{
    int grid_items_count = grid_layout->count();
    int rows_fully_occupied = grid_items_count / 8;
    int last_occupied_column = grid_items_count % 8;

    grid_layout->addWidget(button, rows_fully_occupied + 1, last_occupied_column + 1, {Qt::AlignTop, Qt::AlignLeft});
}

void CMapElementsPlacementWidget::add_map_elements_creation_buttons()
{
    // TODO: This should be resolved by switching to plugin-based map elements architecture
    add_stationary_map_element_creation_button(&CCrossing::create_double_carriageway_car_crossing);
    add_stationary_map_element_creation_button(&CCrossing::create_single_carriageway_car_crossing);

    add_stationary_map_element_creation_button(&CPavement::create_pavement);
    add_stationary_map_element_creation_button(&CPedestrianCrossing::create_pedestrian_crossing);

    add_stationary_map_element_creation_button(&CRoadway::create_one_way_roadway);
    add_stationary_map_element_creation_button(&CRoadway::create_two_way_roadway_with_dotted_line);
    add_stationary_map_element_creation_button(&CRoadway::create_two_way_roadway_with_double_continous_line);

    add_stationary_map_element_creation_button(&CTurn::create_double_carriageway_car_turn_with_single_lane);
    add_stationary_map_element_creation_button(&CTurn::create_single_carriageway_car_turn_with_single_lane);

    add_stationary_map_element_creation_button(&CTrafficLight::create_collision_possible_traffic_lights);

    add_stationary_map_element_creation_button(&CWarningSign::create_equivalent_intersection_sign);
    add_stationary_map_element_creation_button(&CWarningSign::create_give_way_sign);

    add_stationary_map_element_creation_button(&CInformationSign::create_one_wady_road_sign);
    add_stationary_map_element_creation_button(&CInformationSign::create_priority_road_sign);
    add_stationary_map_element_creation_button(&CInformationSign::create_pedestrian_crossing_sign);
    add_stationary_map_element_creation_button(&CInformationSign::create_built_up_area_sign);
    add_stationary_map_element_creation_button(&CInformationSign::create_non_built_up_area_sign);

    add_stationary_map_element_creation_button(&CProhibitionSign::create_left_turn_prohibited_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_right_turn_prohibited_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_turning_prohibited_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_no_entry_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_speed_limit_30_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_speed_limit_50_sign);
    add_stationary_map_element_creation_button(&CProhibitionSign::create_stop_sign);

    add_stationary_map_element_creation_button(&CInjuctionSign::create_drive_straight_sign);
    add_stationary_map_element_creation_button(&CInjuctionSign::create_turn_left_sign);
    add_stationary_map_element_creation_button(&CInjuctionSign::create_turn_right_sign);
}

CMapElementsPlacementWidget::CMapElementsPlacementWidget(CMapCreationPage *map_creation_page, QWidget *parent) :
    QWidget{parent},
    m_map_creation_page(map_creation_page)
{
    this->setParent(parent);
    this->setLayout(&m_main_layout);

    // Add all creation buttons to grid layouts first, then create expandable widgets to view them
    // ONLY IN THIS ORDER for proper UI
    add_map_elements_creation_buttons();
    m_pavement_elements_buttons_layout.setAlignment(Qt::AlignLeft);
    m_information_signs_buttons_layout.setAlignment(Qt::AlignLeft);
    m_traffic_lights_buttons_layout.setAlignment(Qt::AlignLeft);
    m_roadway_elements_buttons_layout.setAlignment(Qt::AlignLeft);
    m_injuction_signs_buttons_layout.setAlignment(Qt::AlignLeft);
    m_warning_signs_buttons_layout.setAlignment(Qt::AlignLeft);
    m_prohibition_signs_buttons_layout.setAlignment(Qt::AlignLeft);

    QFrame *road_elements_widgets_container = new QFrame(this);
    CExpandableWidget *pavement_elements_expandable_widget =
        create_nested_expandable_widget(road_elements_widgets_container, "Pavement", m_pavement_elements_buttons_layout);
    CExpandableWidget *roadway_elements_expandable_widget =
        create_nested_expandable_widget(road_elements_widgets_container, "Roadway", m_roadway_elements_buttons_layout);

    QVBoxLayout *road_elements_layout = new QVBoxLayout();
    QVBoxLayout *road_elements_widgets_layout = new QVBoxLayout();
    CExpandableWidget *road_elements_expandable_widget = new CExpandableWidget("Road elements", 300, this);

    road_elements_widgets_layout->addWidget(pavement_elements_expandable_widget);
    road_elements_widgets_layout->addWidget(roadway_elements_expandable_widget);

    road_elements_widgets_container->setLayout(road_elements_widgets_layout);
    road_elements_layout->addWidget(road_elements_widgets_container);
    road_elements_expandable_widget->set_content_layout(*road_elements_layout);

    QFrame *traffic_control_elements_widgets_container = new QFrame(this);
    CExpandableWidget *traffic_lights_elements_expandable_widget =
        create_nested_expandable_widget(traffic_control_elements_widgets_container, "Traffic lights", m_traffic_lights_buttons_layout);

    QFrame *traffic_signs_elements_widgets_container = new QFrame(traffic_control_elements_widgets_container);
    CExpandableWidget *warning_signs_expandable_widget =
        create_nested_expandable_widget(traffic_signs_elements_widgets_container, "Warning signs", m_warning_signs_buttons_layout);
    CExpandableWidget *information_signs_expandable_widget =
        create_nested_expandable_widget(traffic_signs_elements_widgets_container, "Information signs", m_information_signs_buttons_layout);
    CExpandableWidget *prohibition_signs_expandable_widget =
        create_nested_expandable_widget(traffic_signs_elements_widgets_container, "Prohibition signs", m_prohibition_signs_buttons_layout);
    CExpandableWidget *injuction_signs_expandable_widget =
        create_nested_expandable_widget(traffic_signs_elements_widgets_container, "Injuction signs", m_injuction_signs_buttons_layout);

    QVBoxLayout *traffic_signs_layout = new QVBoxLayout();
    QVBoxLayout *traffic_signs_widgets_layout = new QVBoxLayout();
    CExpandableWidget *traffic_signs_elements_expandable_widget = new CExpandableWidget("Traffic signs", 300, traffic_control_elements_widgets_container);

    traffic_signs_widgets_layout->addWidget(warning_signs_expandable_widget);
    traffic_signs_widgets_layout->addWidget(information_signs_expandable_widget);
    traffic_signs_widgets_layout->addWidget(prohibition_signs_expandable_widget);
    traffic_signs_widgets_layout->addWidget(injuction_signs_expandable_widget);

    traffic_signs_elements_widgets_container->setLayout(traffic_signs_widgets_layout);
    traffic_signs_layout->addWidget(traffic_signs_elements_widgets_container);
    traffic_signs_elements_expandable_widget->set_content_layout(*traffic_signs_layout);

    QVBoxLayout *traffic_control_elements_layout = new QVBoxLayout();
    QVBoxLayout *traffic_control_elements_widgets_layout = new QVBoxLayout();
    CExpandableWidget *traffic_control_elements_expandable_widget = new CExpandableWidget("Traffic control elements", 300, this);

    traffic_control_elements_widgets_layout->addWidget(traffic_lights_elements_expandable_widget);
    traffic_control_elements_widgets_layout->addWidget(traffic_signs_elements_expandable_widget);

    traffic_control_elements_widgets_container->setLayout(traffic_control_elements_widgets_layout);
    traffic_control_elements_layout->addWidget(traffic_control_elements_widgets_container);
    traffic_control_elements_expandable_widget->set_content_layout(*traffic_control_elements_layout);

    m_main_layout.addWidget(road_elements_expandable_widget);
    m_main_layout.addWidget(traffic_control_elements_expandable_widget);
}

CMapElementsPlacementWidget::~CMapElementsPlacementWidget()
{
    auto buttons_list = m_buttons_mapped_to_creators.keys();
    for(auto *button : buttons_list){
        delete button;
    }
}

void CMapElementsPlacementWidget::add_stationary_map_element_creation_button(CStationaryMapElement *(*creation_func)(void))
{
    auto *stationary_map_element = creation_func();
    QPixmap element_pixmap = stationary_map_element->pixmap();
    QIcon button_icon(element_pixmap);
    auto *element_button = new QPushButton(button_icon, "");
    QObject::connect(element_button, &QPushButton::clicked, this, [=]() {
        m_map_creation_page->relay_creator_button_clicked(creation_func);
    });

    QSize default_size = stationary_map_element->get_default_map_element_size();
    element_button->setIconSize(default_size);
    element_button->setToolTip(stationary_map_element->get_description());
    element_button->setFixedSize(default_size.width() + 5, default_size.height() + 5);
    m_buttons_mapped_to_creators.insert(element_button, creation_func);

    EStationaryMapElementType type = stationary_map_element->get_map_element_type();
    if(type == EStationaryMapElementType::road_element){
        auto *road_element = static_cast<CRoadElement*>(stationary_map_element);
        if(road_element->get_permitted_road_users() == EPermittedRoadUsers::pedestrians){
            add_button_to_grid_layout(element_button, &m_pavement_elements_buttons_layout);
        }
        else{
            add_button_to_grid_layout(element_button, &m_roadway_elements_buttons_layout);
        }
    }
    else{
        auto *traffic_control_element = static_cast<CTrafficControlElement*>(stationary_map_element);
        if(traffic_control_element->get_traffic_control_element_type() == ETrafficControlElementType::traffic_lights){
            add_button_to_grid_layout(element_button, &m_traffic_lights_buttons_layout);
        }
        else{
            auto *traffic_sign = static_cast<CTrafficSign*>(stationary_map_element);
            ETrafficSignCategory sign_category = traffic_sign->get_traffic_sign_category();

            if(sign_category == ETrafficSignCategory::information_sign)
                add_button_to_grid_layout(element_button, &m_information_signs_buttons_layout);
            else if(sign_category == ETrafficSignCategory::warning_sign)
                add_button_to_grid_layout(element_button, &m_warning_signs_buttons_layout);
            else if(sign_category == ETrafficSignCategory::prohibition_sign)
                add_button_to_grid_layout(element_button, &m_prohibition_signs_buttons_layout);
            else
                add_button_to_grid_layout(element_button, &m_injuction_signs_buttons_layout);
        }
    }

    delete stationary_map_element;
}
