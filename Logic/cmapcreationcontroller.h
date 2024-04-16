#ifndef CMAPCREATIONCONTROLLER_H
#define CMAPCREATIONCONTROLLER_H

#include "cbasecontroller.h"

/**
 * @brief The CMapCreationController class is a controller responsible for the map creation process.
 */
class CMapCreationController : public CBaseController
{
public:
    explicit CMapCreationController();
    CMapCreationController(CEditableMap *map_model);

    ~CMapCreationController();
};

#endif // CMAPCREATIONCONTROLLER_H
