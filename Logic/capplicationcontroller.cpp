#include <QFile>
#include <QFileInfo>

#include "capplicationcontroller.h"
#include "UI/cmainwindow.h"

CApplicationController::CApplicationController()
{
    m_map_creation_controller = new CMapCreationController();
    m_main_window = new CMainWindow(nullptr, this);
    m_main_window->show();
}

CApplicationController::~CApplicationController()
{
    delete m_main_window;
}
