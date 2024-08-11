#include "csimulationcontroller.h"

CSimulationController::CSimulationController(CApplicationController *application_controller, CEditableMap *map_model) :
    CBaseController(application_controller, map_model)
{

}

CSimulationController::~CSimulationController()
{
    if(m_map_model != nullptr){
        delete m_map_model;
    }

    if(m_map_view != nullptr){
        delete m_map_view;
    }
}
