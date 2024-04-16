#include "cbasecontroller.h"

CBaseController::CBaseController()
{
    m_map_model = new CEditableMap();
}

CBaseController::CBaseController(CEditableMap *map_model)
{
    m_map_model = map_model;
}

void CBaseController::set_model(CEditableMap *map_model)
{
    auto *temp = map_model;
    m_map_model = map_model;
    m_map_view->set_model(m_map_model);

    if(temp != nullptr){
        temp->deleteLater();
    }
}

