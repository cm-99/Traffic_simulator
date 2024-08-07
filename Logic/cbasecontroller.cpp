#include "cbasecontroller.h"

//TODO: Do poprawy - CBaseController nie powinien jednak tworzyć mapy
CBaseController::CBaseController()
{
    m_map_model = new CEditableMap(1920, 1080);
}

CBaseController::CBaseController(CEditableMap *map_model)
{
    m_map_model = map_model;
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

