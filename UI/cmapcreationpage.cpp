#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QComboBox>

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
    QPushButton *resize_map_button = new QPushButton("Resize map", this);
    QPushButton *clear_map_button = new QPushButton("Clear map", this);

    int height_hint = save_map_button->sizeHint().height();
    save_map_button->setFixedSize(200, height_hint);
    configure_simulation_button->setFixedSize(200, height_hint);
    clear_map_button->setFixedSize(200, height_hint);


    connect(save_map_button, &QPushButton::clicked, this, &CMapCreationPage::relay_map_saving_request);
    connect(clear_map_button, &QPushButton::clicked, this, &CMapCreationPage::process_map_clearing_request);
    connect(configure_simulation_button, &QPushButton::clicked, this, &CMapCreationPage::relay_simulation_start_request);
    connect(resize_map_button, &QPushButton::clicked, this, &CMapCreationPage::relay_map_resize_request);

    buttons_layout->addWidget(resize_map_button);
    buttons_layout->addWidget(clear_map_button);
    buttons_layout->addWidget(save_map_button);
    buttons_layout->insertSpacerItem(2, new QSpacerItem(0,height_hint,QSizePolicy::Expanding,QSizePolicy::Fixed));
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

void CMapCreationPage::relay_map_resize_request()
{
    QDialog map_resize_dialog;
    auto main_layout = new QVBoxLayout();

    auto map_dimensions_combobox = new QComboBox();
    map_dimensions_combobox->setEditable(false);
    
    auto map_sizes = m_map_creation_controller->get_supported_map_sizes();
    for(auto size : *map_sizes){
        map_dimensions_combobox->addItem(QString::number(size.width()) + " x " + QString::number(size.height()), size);
    }

    auto button_box = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(button_box, &QDialogButtonBox::accepted, &map_resize_dialog, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, &map_resize_dialog, &QDialog::reject);

    main_layout->addWidget(map_dimensions_combobox);
    main_layout->addWidget(button_box);
    map_resize_dialog.setLayout(main_layout);

    if(map_resize_dialog.exec() == QDialog::DialogCode::Accepted){
        QSize size_chosen = map_dimensions_combobox->currentData().toSize();
        m_map_creation_controller->resize_map(size_chosen.width(), size_chosen.height());
    }
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

void CMapCreationPage::process_map_clearing_request()
{
    QMessageBox msg_box;
    msg_box.setText("Do you want to clear the map? All unsaved changes will be lost.");
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg_box.setDefaultButton(QMessageBox::Yes);
    int button_clicked = msg_box.exec();

    if(button_clicked == QMessageBox::Yes){
        m_map_creation_controller->clear_map();
    }
}
