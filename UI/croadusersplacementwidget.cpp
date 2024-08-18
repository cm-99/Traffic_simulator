#include "croadusersplacementwidget.h"
#include "Logic/MapModels/creadonlymap.h"
#include "MapElements/RoadUsers/ccar.h"
#include "MapElements/RoadUsers/cpedestrian.h"
#include "UI/cexpandablewidget.h"
#include "UI/csimulationpage.h"

CRoadUsersPlacementWidget::CRoadUsersPlacementWidget(CSimulationPage *simulation_page, QWidget *parent) :
    QWidget{parent},
    m_simulation_page(simulation_page)
{
    auto main_wrapper_layout = new QVBoxLayout(this);
    auto main_frame = new QFrame();
    auto main_layout = new QVBoxLayout();
    this->setParent(parent);
    this->setLayout(main_wrapper_layout);

    add_road_users_creation_buttons();
    m_pedestrians_buttons_layout.setAlignment(Qt::AlignLeft);
    m_cars_buttons_layout.setAlignment(Qt::AlignLeft);

    QFrame *cars_content_widget = new QFrame();
    QVBoxLayout *cars_wrapper_layout = new QVBoxLayout();
    CExpandableWidget *cars_expandable_widget = new CExpandableWidget("Cars", 300, main_frame);

    cars_content_widget->setLayout(&m_cars_buttons_layout);
    cars_wrapper_layout->addWidget(cars_content_widget);
    cars_expandable_widget->set_content_layout(*cars_wrapper_layout);

    QFrame *pedestrians_content_widget = new QFrame();
    QVBoxLayout *pedestrians_wrapper_layout = new QVBoxLayout();
    CExpandableWidget *pedestrians_expandable_widget = new CExpandableWidget("Pedestrians", 300, main_frame);

    pedestrians_content_widget->setLayout(&m_pedestrians_buttons_layout);
    pedestrians_wrapper_layout->addWidget(pedestrians_content_widget);
    pedestrians_expandable_widget->set_content_layout(*pedestrians_wrapper_layout);

    main_layout->addWidget(cars_expandable_widget);
    main_layout->addWidget(pedestrians_expandable_widget);

    main_frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    main_frame->setLayout(main_layout);
    main_wrapper_layout->addWidget(main_frame);
}


void CRoadUsersPlacementWidget::add_road_users_creation_buttons()
{
    add_road_user_creation_button(&CCar::create_car);
    add_road_user_creation_button(&CPedestrian::create_pedestrian);
}

void CRoadUsersPlacementWidget::add_button_to_grid_layout(QPushButton *button, QGridLayout *grid_layout)
{
    int grid_items_count = grid_layout->count();
    int rows_fully_occupied = grid_items_count / 8;
    int last_occupied_column = grid_items_count % 8;

    grid_layout->addWidget(button, rows_fully_occupied + 1, last_occupied_column + 1, {Qt::AlignTop, Qt::AlignLeft});
}

CRoadUsersPlacementWidget::~CRoadUsersPlacementWidget()
{
    auto buttons_list = m_buttons_mapped_to_creators.keys();
    for(auto *button : buttons_list){
        delete button;
    }
    buttons_list.clear();
}

void CRoadUsersPlacementWidget::add_road_user_creation_button(CRoadUser *(*creation_func)(void))
{
    auto road_user = creation_func();
    QPixmap element_pixmap = road_user->pixmap();
    QIcon button_icon(element_pixmap);
    auto *road_user_button = new QPushButton(button_icon, "");
    QObject::connect(road_user_button, &QPushButton::clicked, this, [=]() {
        m_simulation_page->relay_creator_button_clicked(creation_func);
    });

    QSize default_size = CReadOnlyMap::get_default_cell_size();
    road_user_button->setIconSize(default_size);
    road_user_button->setToolTip(road_user->get_description());
    road_user_button->setFixedSize(default_size.width() + 5, default_size.height() + 5);
    m_buttons_mapped_to_creators.insert(road_user_button, creation_func);

    ERoadUsers type = road_user->get_road_user_type();
    if(type == ERoadUsers::car){
        add_button_to_grid_layout(road_user_button, &m_cars_buttons_layout);
    }
    else{
        add_button_to_grid_layout(road_user_button, &m_pedestrians_buttons_layout);
    }

    delete road_user;
}
