#include "cmainwindow.h"

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    this->setMinimumSize(800, 450);
    this->setCentralWidget(&home_page);
    home_page.show();
}

CMainWindow::~CMainWindow()
{

}

void CMainWindow::slot_configure_simulation()
{

}

void CMainWindow::slot_show_map_creation_page()
{

}

void CMainWindow::slot_show_map_selection_page()
{

}

void CMainWindow::slot_show_map_file_dialog()
{

}
