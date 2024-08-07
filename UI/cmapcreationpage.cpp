#include <QFileDialog>
#include <QApplication>

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
    QPushButton *configure_simulation_button = new QPushButton("Configure simulation", this);


    connect(save_map_button, &QPushButton::clicked, this, &CMapCreationPage::relay_map_saving_request);
    connect(configure_simulation_button, &QPushButton::clicked, this, &CMapCreationPage::relay_simulation_start_request);

    buttons_layout->addWidget(save_map_button);
    buttons_layout->addWidget(configure_simulation_button);

    m_main_layout.addLayout(buttons_layout);
    m_main_layout.addWidget(m_map_creation_controller->get_view());
    m_main_layout.addWidget(m_map_elements_placement_widget);
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

void CMapCreationPage::relay_map_saving_request()
{
    QString map_file_path;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(qApp -> applicationDirPath());
    dialog.setNameFilters(m_map_creation_controller->get_supported_map_file_formats());
    dialog.setWindowTitle("Map saving");

    QStringList file_paths = QStringList();

    if (dialog.exec())
        file_paths = dialog.selectedFiles();

    if (file_paths.size() > 0)
        map_file_path = file_paths.at(0);
    else
        return;

    m_map_creation_controller->delegate_map_saving(map_file_path);
}

void CMapCreationPage::relay_simulation_start_request()
{
    m_map_creation_controller->process_simulation_start_request();
}
