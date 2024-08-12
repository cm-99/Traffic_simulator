#include <QFileDialog>
#include <QApplication>

#include "cmainwindow.h"

CMainWindow::CMainWindow(QWidget *parent, CApplicationController *application_controller):
    QMainWindow{parent},
    m_map_selection_page(this),
    m_map_creation_page(application_controller->get_map_creation_controller(), this)
{
    QSize size_hint = m_back_button.sizeHint();
    QPixmap back_pixmap(":/graphics/buttons_icons/back_button.png");
    back_pixmap = back_pixmap.scaled(size_hint, Qt::KeepAspectRatio);
    QIcon icon(back_pixmap);
    m_back_button.setIcon(icon);

    this->m_application_controller = application_controller;
    this->menuBar()->setCornerWidget(&m_back_button);
    this->menuBar()->setCornerWidget(&m_home_page_button, Qt::TopLeftCorner);
    this->setMinimumSize(800, 450);
    m_home_page_button.setText("Main menu");
    m_back_button.setDisabled(true);

    connect(&m_home_page_button, &QPushButton::clicked, this, &CMainWindow::slot_back_to_home_page);
    connect(&m_back_button, &QPushButton::clicked, this, &CMainWindow::slot_switch_to_previous_page);
    connect(&m_home_page, &CHomePage::signal_map_creator_requested, this, &CMainWindow::slot_show_map_creation_page);
    connect(&m_home_page, &CHomePage::signal_map_selection_requested, this, &CMainWindow::slot_show_map_selection_page);
    connect(&m_home_page, &CHomePage::signal_map_loading_requested, this, &CMainWindow::slot_process_map_loading_request);

    stacked_widget.addWidget(&m_home_page);
    stacked_widget.addWidget(&m_map_selection_page);
    stacked_widget.addWidget(&m_map_creation_page);


    this->setCentralWidget(&stacked_widget);
    stacked_widget.setCurrentWidget(&m_home_page);
}

CMainWindow::~CMainWindow(){}

void CMainWindow::relay_simulation_start_request(QString map_file_path)
{
    m_application_controller->process_simulation_start_request(map_file_path);
}

SSimulationConfiguration CMainWindow::get_simulation_configuration_from_user()
{
    CSimulationConfigurationDialog configuration_dialog;
    if(configuration_dialog.exec() == QDialog::DialogCode::Accepted){
        return configuration_dialog.get_simulation_configuration();
    }
    else{
        return SSimulationConfiguration();
    }
}

void CMainWindow::show_simulation_page()
{

}

void CMainWindow::slot_show_map_creation_page()
{
    m_back_button.setDisabled(false);
    previous_page_index = stacked_widget.currentIndex();
    stacked_widget.setCurrentWidget(&m_map_creation_page);
}

void CMainWindow::slot_show_map_selection_page()
{
    m_back_button.setDisabled(false);
    previous_page_index = stacked_widget.currentIndex();
    stacked_widget.setCurrentWidget(&m_map_selection_page);
}

void CMainWindow::slot_process_map_loading_request()
{
    QString map_file_path;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(qApp -> applicationDirPath());
    dialog.setNameFilters(m_application_controller->get_supported_map_file_formats());
    dialog.setWindowTitle("Load file");

    QStringList file_paths = QStringList();

    if (dialog.exec())
        file_paths = dialog.selectedFiles();

    if (file_paths.size() > 0)
        map_file_path = file_paths.at(0);
    else
        return;

    slot_open_map_in_creator(map_file_path);
}

void CMainWindow::slot_switch_to_previous_page()
{
    int page_index = stacked_widget.currentIndex();
    IUIPage *current_page = qobject_cast<IUIPage*>(stacked_widget.currentWidget());
    if (current_page)
        current_page->restore_to_default();

    stacked_widget.setCurrentIndex(previous_page_index);
    CHomePage *home_page = qobject_cast<CHomePage*>(stacked_widget.currentWidget());
    if(home_page)
        m_back_button.setDisabled(true);

    previous_page_index = page_index;
}

void CMainWindow::slot_back_to_home_page()
{
    int page_index = stacked_widget.currentIndex();
    IUIPage *current_page = qobject_cast<IUIPage*>(stacked_widget.currentWidget());
    if (current_page)
        current_page->restore_to_default();

    int home_page_index = stacked_widget.indexOf(&m_home_page);
    stacked_widget.setCurrentIndex(home_page_index);
    m_back_button.setDisabled(true);

    previous_page_index = page_index;
}

void CMainWindow::slot_open_map_in_creator(QString map_file_path)
{
    bool loading_successful = m_application_controller -> delegate_map_loading_into_creator(map_file_path);
    if(loading_successful){
        slot_show_map_creation_page();
    }
}
