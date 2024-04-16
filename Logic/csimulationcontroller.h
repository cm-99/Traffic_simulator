#ifndef CSIMULATIONCONTROLLER_H
#define CSIMULATIONCONTROLLER_H

#include "cbasecontroller.h"

/**
 * @brief The CSimulationController class is a controller responsible for the traffic simulation process.
 *
 * The simulation is performed in discrete steps determined by a timer, the timeout of which is transmitted to the model
 * to update the state of the simulation.
 */
class CSimulationController : public CBaseController
{
public:
    explicit CSimulationController();
    CSimulationController (CEditableMap *map_model);

    ~CSimulationController();
};

#endif // CSIMULATIONCONTROLLER_H
