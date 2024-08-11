#include <QFile>
#include <QFileInfo>

#include "capplicationcontroller.h"
#include "UI/cmainwindow.h"

CApplicationController::CApplicationController()
{
    m_map_creation_controller = new CMapCreationController(this);
    m_main_window = new CMainWindow(nullptr, this);
    m_main_window->show();
}

CApplicationController::~CApplicationController()
{
    delete m_main_window;
}

bool CApplicationController::delegate_map_loading_into_creator(QString map_file_path)
{
    bool loading_successful = m_map_creation_controller->load_map_into_creator(map_file_path);
    return loading_successful;
}

bool CApplicationController::process_simulation_start_request(QString map_file_path)
{

}

