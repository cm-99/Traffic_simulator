#include "cbaseview.h"

CBaseView::CBaseView(CReadOnlyMap *map_model)
{
    set_model(map_model);
}

void CBaseView::set_model(CReadOnlyMap *map_model)
{
    m_map_model = map_model;
    setScene(map_model);
}
