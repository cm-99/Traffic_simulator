#ifndef CBASECONTROLLER_H
#define CBASECONTROLLER_H

#include "Logic/MapModels/ceditablemap.h"
#include "Logic/MapViews/cbaseview.h"

/**
 * @brief The CBaseController class is base class for all controllers dealing with the map models.
 */
class CBaseController
{
public:
    /**
     * @brief CBaseController is an explicit constructor for CBaseController. It creats a new, empty map model.
     */
    explicit CBaseController();
    /**
     * @brief CBaseController is an constructor for CBaseController which takes over control of an already existing map model.
     */
    CBaseController(CEditableMap *map_model);
    virtual ~CBaseController() {};

    void set_model(CEditableMap *map_model);

protected:
    CEditableMap *m_map_model;
    CBaseView *m_map_view;
};

#endif // CBASECONTROLLER_H
