#include <QDir>
#include <QApplication>

#include "UI/cmainwindow.h"
#include "cmapselectionpage.h"

CMapSelectionPage::CMapSelectionPage(CMainWindow *parent):
    m_main_window(parent)
{
    this->setParent(parent);

    m_button_edit_map.setText("Edit map");
    m_button_edit_map.setDisabled(true);
    m_button_edit_map.setFixedSize(200, 50);

    connect(&m_button_edit_map, &QPushButton::clicked, this, [=](){
        m_main_window->slot_open_map_in_creator(m_current_map_file_path);
    });

    m_button_configure_simulation.setText("Configure simulation");
    m_button_configure_simulation.setDisabled(true);
    m_button_configure_simulation.setFixedSize(200, 50);

    connect(&m_button_configure_simulation, &QPushButton::clicked, this, [=](){
        m_main_window->relay_simulation_start_request(m_current_map_file_path);
        restore_to_default();
    });

    QDir directory(":graphics/map_selection");
    QStringList map_graphics_list = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);

    int map_graphics_list_size = map_graphics_list.size();
    for(int i = 0; i < map_graphics_list_size; i++){

        QString graphic_file_path = directory.filePath(map_graphics_list[i]);
        QString map_file_path = qApp->applicationDirPath() + "/map_selection_files/" + map_graphics_list[i].split(".").first() + ".xml";

        auto map_button = new QPushButton(this);
        QPixmap pixmap(graphic_file_path);
        QSize pixmap_original_size = pixmap.rect().size();
        pixmap = pixmap.scaled(QSize(pixmap_original_size.width()/4, pixmap_original_size.height()/4), Qt::KeepAspectRatio);

        QIcon icon(pixmap);
        map_button->setIcon(icon);
        map_button->setIconSize(pixmap.rect().size());
        map_button->setFixedSize(pixmap.rect().size().width() + 5, pixmap.rect().size().height() + 5);

        m_map_miniatures_layout.addWidget(map_button);

        if(i != map_graphics_list_size - 1){
            m_map_miniatures_layout.addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
        }

        connect(map_button, &QPushButton::clicked, this, [=](){
            map_button->setDown(true);

            if(!m_button_edit_map.isEnabled()){
                m_button_edit_map.setDisabled(false);
                m_button_configure_simulation.setDisabled(false);
            }

            m_current_map_file_path = map_file_path;
        });
    }


    m_buttons_layout.addWidget(&m_button_edit_map);
    m_buttons_layout.insertSpacerItem(1, new QSpacerItem(0,50,QSizePolicy::Expanding,QSizePolicy::Fixed));
    m_buttons_layout.addWidget(&m_button_configure_simulation);

    m_main_layout.addLayout(&m_map_miniatures_layout);
    m_main_layout.addLayout(&m_buttons_layout);

    this -> setLayout(&m_main_layout);
}

void CMapSelectionPage::restore_to_default()
{
    m_button_edit_map.setDisabled(true);
    m_button_configure_simulation.setDisabled(true);
    m_current_map_file_path.clear();
}
