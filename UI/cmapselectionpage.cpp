#include "cmapselectionpage.h"

CMapSelectionPage::CMapSelectionPage(QWidget *parent)
{
    this->setParent(parent);

    m_button_edit_map.setFixedSize(200, 50);
    m_button_start_simulation.setFixedSize(200, 50);
    m_button_edit_map.setText("Edit");
    m_button_start_simulation.setText("Start simulation");
    m_buttons_layout.addWidget(&m_button_edit_map);
    m_buttons_layout.addSpacing(30);
    m_buttons_layout.addWidget(&m_button_start_simulation);

    m_main_layout.addLayout(&m_map_miniatures_layout);
    m_main_layout.addLayout(&m_buttons_layout);

    this -> setLayout(&m_main_layout);
}
