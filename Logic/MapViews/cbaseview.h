#ifndef CBASEVIEW_H
#define CBASEVIEW_H

#include <QGraphicsView>

#include "Logic/MapModels/creadonlymap.h"
/**
 * @brief The CBaseView class provides a widget for displaying the contents of traffic map model.
 */
class CBaseView : public QGraphicsView
{
public:
    CBaseView(CReadOnlyMap* map_model);
    void set_model(CReadOnlyMap *map_model);

private:
    const CReadOnlyMap* m_map_model;
};

#endif // CBASEVIEW_H
