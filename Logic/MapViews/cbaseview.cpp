#include "cbaseview.h"

CBaseView::CBaseView(CReadOnlyMap *map_model)
{
    set_model(map_model);
}

void CBaseView::set_model(CReadOnlyMap *map_model)
{
    if(map_model == nullptr){
        throw std::invalid_argument("Argument map_model must be a valid object");
    }

    m_map_model = map_model;
    setScene(map_model);
}
