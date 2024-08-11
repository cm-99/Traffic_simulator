#ifndef CBASECONTROLLER_H
#define CBASECONTROLLER_H

#include "Logic/MapModels/ceditablemap.h"
#include "Logic/MapViews/cbaseview.h"

class CApplicationController;

/**
 * @brief The CBaseController class is base class for all controllers dealing with the map models.
 */
class CBaseController: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief CBaseController is an explicit constructor for CBaseController.
     */
    explicit CBaseController(CApplicationController *application_controller);
    /**
     * @brief CBaseController is an constructor for CBaseController which takes over control of an already existing map model.
     */
    CBaseController(CApplicationController *application_controller, CEditableMap *map_model);
    virtual ~CBaseController() {};

    CBaseView *get_view() const {return m_map_view;}
    void set_model(CEditableMap *map_model);

protected:
    CEditableMap *m_map_model;
    CBaseView *m_map_view;
    CApplicationController *m_application_controller;
};

#endif // CBASECONTROLLER_H
