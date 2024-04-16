#include "cmapcreationcontroller.h"

CMapCreationController::CMapCreationController(CEditableMap *map_model) :
    CBaseController(map_model)
{

}

CMapCreationController::~CMapCreationController()
{
    if(m_map_model != nullptr){
        delete m_map_model;
    }

    if(m_map_view != nullptr){
        delete m_map_view;
    }
}
