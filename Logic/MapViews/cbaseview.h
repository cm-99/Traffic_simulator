#ifndef CBASEVIEW_H
#define CBASEVIEW_H

#include <QGraphicsView>

#include "Logic/MapModels/creadonlymap.h"
/**
 * @brief The CBaseView class provides a widget (view) for displaying the contents of traffic map model.
 */
class CBaseView : public QGraphicsView
{
public:
    CBaseView(CReadOnlyMap* map_model);
    /**
     * @brief set_model - Sets (or if already set, changes) the map model displayed
     * @param map_model - map model to be set
     */
    void set_model(CReadOnlyMap *map_model);

protected:
    const CReadOnlyMap* m_map_model;
};

#endif // CBASEVIEW_H
