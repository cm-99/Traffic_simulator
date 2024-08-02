#include "cmapcreationpage.h"
#include "cexpandablewidget.h"

CMapCreationPage::CMapCreationPage(CMapCreationController *map_creation_controller, QWidget *parent) :
    m_map_creation_controller(map_creation_controller)
{
    this->setParent(parent);
    this->setLayout(&m_main_layout);
    m_map_elements_placement_widget = new CMapElementsPlacementWidget(this);

    QHBoxLayout *buttons_layout = new QHBoxLayout();
    QPushButton *save_map_button = new QPushButton("Save map", this);
    QPushButton *start_simulation_button = new QPushButton("Start simulation", this);

    buttons_layout->addWidget(save_map_button);
    buttons_layout->addWidget(start_simulation_button);

    m_main_layout.addWidget(m_map_creation_controller->get_view());
    m_main_layout.addWidget(m_map_elements_placement_widget);
    m_main_layout.addLayout(buttons_layout);
}

void CMapCreationPage::restore_to_default()
{
    auto expandable_widgets_list = m_map_elements_placement_widget->findChildren<CExpandableWidget*>(Qt::FindChildrenRecursively);
    for (auto *expandable_widget : expandable_widgets_list){
        expandable_widget->slot_toggle(false);
    }
}

void CMapCreationPage::relay_creator_button_clicked(CStationaryMapElement *(*creation_func)())
{
    m_map_creation_controller->set_element_to_place_creation_func(creation_func);
}
