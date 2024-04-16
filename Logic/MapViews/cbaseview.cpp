#include "cbaseview.h"

CBaseView::CBaseView(CReadOnlyMap *map_model)
{
    set_model(map_model);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(map_model -> width(), map_model -> height());
    setScene(map_model);
}

void CBaseView::set_model(CReadOnlyMap *map_model)
{
    m_map_model = map_model;
    setScene(map_model);
}
