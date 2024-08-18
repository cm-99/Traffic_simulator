#include <QFile>
#include <QFileInfo>
#include <QObject>

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
    delete m_map_creation_controller;
    if(m_simulation_controller != nullptr){
        delete m_simulation_controller;
    }

    delete m_main_window;
}

bool CApplicationController::delegate_map_loading_into_creator(QString map_file_path)
{
    bool loading_successful = m_map_creation_controller->load_map_into_creator(map_file_path);
    return loading_successful;
}

bool CApplicationController::process_simulation_start_request(QString map_file_path)
{
    CEditableMap *simulation_map = m_map_creation_controller->load_map_from_file(map_file_path);

    if(simulation_map == nullptr){
        return false;
    }

    auto simulation_configuration = m_main_window->get_simulation_configuration_from_user();
    if(simulation_configuration.is_empty()){
        return false;
    }

    if(m_simulation_controller == nullptr){
        m_simulation_controller = new CSimulationController(this, simulation_map, simulation_configuration);
    }
    m_main_window->show_simulation_page();

    return true;
}

