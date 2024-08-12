#ifndef CSIMULATIONVIEW_H
#define CSIMULATIONVIEW_H

#include "cbaseview.h"
#include "Logic/csimulationcontroller.h"

/**
 * @brief The CSimulationView class provides a widget for displaying the contents of traffic map model during traffic simulation
 * and is also an access layer providing the user ability to change the parameters of those contents.
 */
class CSimulationView: public CBaseView
{
public:
    CSimulationView(CReadOnlyMap *map_model, CSimulationController *simulation_controller);

private:
    CSimulationController *m_simulation_controller;
};

#endif // CSIMULATIONVIEW_H
