#include "csimulationview.h"

CSimulationView::CSimulationView(CReadOnlyMap *map_model, CSimulationController *simulation_controller) :
    CBaseView(map_model),
    m_simulation_controller(simulation_controller)
{
    setMouseTracking(true);
}
