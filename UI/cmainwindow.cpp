#include <QFileDialog>
#include <QApplication>

#include "cmainwindow.h"

CMainWindow::CMainWindow(QWidget *parent, CApplicationController *application_controller)
    : QMainWindow{parent}
{
    this -> m_application_controller = application_controller;
    QPushButton *back_button = new QPushButton("Back", this);
    this->menuBar()->setCornerWidget(back_button);
    this->setMinimumSize(800, 450);

    connect(back_button, &QPushButton::clicked, this, &CMainWindow::slot_switch_to_previous_page);
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

void CMainWindow::slot_show_map_creation_page()
{
    previous_page_index = stacked_widget.currentIndex();
    stacked_widget.setCurrentWidget(&m_map_creation_page);
}

void CMainWindow::slot_show_map_selection_page()
{
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

    m_application_controller -> delegate_map_loading(map_file_path);
}

void CMainWindow::slot_switch_to_previous_page()
{
    int temp = stacked_widget.currentIndex();
    IUIPage *current_page = qobject_cast<IUIPage*>(stacked_widget.currentWidget());
    if (current_page)
        current_page->restore_to_default();
    stacked_widget.setCurrentIndex(previous_page_index);
    previous_page_index = temp;
}


