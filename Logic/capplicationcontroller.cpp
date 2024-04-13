#include "capplicationcontroller.h"
#include "UI/cmainwindow.h"

CApplicationController::CApplicationController()
{
    m_main_window = new CMainWindow(nullptr, this);
    m_main_window->show();
}

CApplicationController::~CApplicationController()
{
    delete m_main_window;
}

const QList<QString>& CApplicationController::get_supported_map_file_formats() const
{
    return m_supported_map_file_formats;
}

void CApplicationController::delegate_map_loading(QString map_file_path)
{

}

void CApplicationController::delegate_map_saving(QString map_file_path)
{

}

void CApplicationController::set_supported_map_file_formats()
{

}
