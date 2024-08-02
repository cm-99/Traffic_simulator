#include "cturn.h"

CTurn::CTurn(int carriageways_number, int lanes_number, QPixmap turn_pixmap, EPermittedRoadUsers permitted_road_users, QString description,
             EHorizontalMoveDirection horizontal_direction, EVerticalMoveDirection vertical_direction) :
    CRoadwayElement(carriageways_number, lanes_number, ERoadwayElementType::turn, EMovementPlane::horizontal, permitted_road_users, description)
{
    setPixmap(turn_pixmap);
    m_horizontal_direction = horizontal_direction;
    m_vertical_direction = vertical_direction;
}

CStationaryMapElement *CTurn::create_double_carriageway_car_turn_with_single_lane()
{
    return new CTurn(2, 1, QPixmap(":/map_elements_graphics/turns/double_carriageway_single_lane_turn.png"), EPermittedRoadUsers::cars,
    "Double carriageway turn with single lane");
}

CStationaryMapElement *CTurn::create_single_carriageway_car_turn_with_single_lane()
{
    return new CTurn(1, 1, QPixmap(":/map_elements_graphics/turns/single_carriageway_single_lane_turn.png"), EPermittedRoadUsers::cars,
                     "Single carriageway turn with single lane");
}
