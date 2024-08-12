#include "csimulationcontroller.h"
#include "MapViews/csimulationview.h"

CSimulationController::CSimulationController(CApplicationController *application_controller, CEditableMap *map_model, SSimulationConfiguration simulation_configuration) :
    CBaseController(application_controller, map_model),
    m_simulation_configuration(simulation_configuration)
{
    m_map_view = new CSimulationView(m_map_model, this);
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
