#include "cbasecontroller.h"

CBaseController::CBaseController(CApplicationController *application_controller):
    m_application_controller(application_controller)
{}

CBaseController::CBaseController(CApplicationController *application_controller, CEditableMap *map_model):
    m_application_controller(application_controller)
{
    set_model(map_model);
}

void CBaseController::set_model(CEditableMap *map_model)
{
    auto temp = m_map_model;
    m_map_model = map_model;
    m_map_view->set_model(m_map_model);

    if(temp != nullptr){
        temp->deleteLater();
    }
}

