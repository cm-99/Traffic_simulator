#ifndef CMAPCREATIONVIEW_H
#define CMAPCREATIONVIEW_H

#include "Logic/cmapcreationcontroller.h"
#include "cbaseview.h"

/**
 * @brief The CMapCreationView class provides a widget for displaying and editing the contents of traffic map model during map creation.
 *
 * It provides access layer that allows the user to add/remove map elements.
 * User actions are relayed to controller responsible for map creation to be validated.
 */
class CMapCreationView : public CBaseView
{
public:
    CMapCreationView(CReadOnlyMap *map_model, CMapCreationController *map_creation_controller);

private:
    CMapCreationController *m_map_creation_controller;

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void process_zooming(QWheelEvent *event);

    void place_element_and_prepare_next(QMouseEvent *event);
};

#endif // CMAPCREATIONVIEW_H
