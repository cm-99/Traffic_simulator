#ifndef CMAPCREATIONVIEW_H
#define CMAPCREATIONVIEW_H

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
    CMapCreationView(CReadOnlyMap *map_model);
};

#endif // CMAPCREATIONVIEW_H
