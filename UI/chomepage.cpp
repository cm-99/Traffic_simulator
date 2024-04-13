#include "chomepage.h"

CHomePage::CHomePage(QWidget *parent)
{
    this->setParent(parent);
    m_buttons_layout.setAlignment(Qt::AlignHCenter);

    m_button_select_map.setFixedSize(300, 50);
    m_button_load_map_file.setFixedSize(300, 50);
    m_button_create_new_map.setFixedSize(300, 50);

    m_button_select_map.setText("Select map");
    m_button_load_map_file.setText("Load map file");
    m_button_create_new_map.setText("Create a new map");

    m_buttons_layout.addSpacing(30);
    m_buttons_layout.addWidget(&m_button_select_map);
    m_buttons_layout.addSpacing(30);
    m_buttons_layout.addWidget(&m_button_load_map_file);
    m_buttons_layout.addSpacing(30);
    m_buttons_layout.addWidget(&m_button_create_new_map);
    m_buttons_layout.addSpacing(30);

    this->setLayout(&m_buttons_layout);

    // Connect buttons to the relays
    connect(&m_button_create_new_map, &QPushButton::clicked, this, &CHomePage::signal_map_creator_requested);
    connect(&m_button_load_map_file, &QPushButton::clicked, this, &CHomePage::signal_map_loading_requested);
    connect(&m_button_select_map, &QPushButton::clicked, this, &CHomePage::signal_map_selection_requested);
}
