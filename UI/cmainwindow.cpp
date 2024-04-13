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
    this->setCentralWidget(&m_home_page);
    m_home_page.show();

    connect(&m_home_page, &CHomePage::signal_map_creator_requested, this, &CMainWindow::slot_show_map_creation_page);
    connect(&m_home_page, &CHomePage::signal_map_selection_requested, this, &CMainWindow::slot_show_map_selection_page);
    connect(&m_home_page, &CHomePage::signal_map_loading_requested, this, &CMainWindow::slot_process_map_loading_request);
}

CMainWindow::~CMainWindow(){}

void CMainWindow::slot_show_map_creation_page()
{
    this->setCentralWidget(&m_map_creation_page);
}

void CMainWindow::slot_show_map_selection_page()
{
    this->setCentralWidget(&m_map_selection_page);
    m_map_selection_page.show();
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

    m_application_controller->delegate_map_loading(map_file_path);
}


