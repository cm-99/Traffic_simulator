#ifndef CTURN_H
#define CTURN_H

#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"
#include "MapElements/croaduser.h"

/**
 * @brief The CTurn class
 *
 * Looking from the persepctive of road user moving from left to right it should always start as a right/downward turn.
 */
class CTurn : public CRoadwayElement
{
public:
    static CStationaryMapElement *create_double_carriageway_car_turn_with_single_lane();
    static CStationaryMapElement *create_single_carriageway_car_turn_with_single_lane();

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

    EHorizontalMoveDirection get_horizontal_direction_possible() const {return m_horizontal_direction;}
    EVerticalMoveDirection get_vertical_direction_possible() const {return m_vertical_direction;}
    /**
     * @brief set_horizontal_direction_possible is a setter method for m_horizontal_direction possible.
     * Should be used when rotating the turn during map creation.
     * @param new_horizontal_direction_possible
     */
    void set_horizontal_direction_possible(EHorizontalMoveDirection horizontal_direction)
    {m_horizontal_direction = horizontal_direction;}

    void set_vertical_direction_possible(EVerticalMoveDirection vertical_direction)
    {m_vertical_direction = vertical_direction;}

private:
    CTurn(int carriageways_number, int lanes_number, QPixmap turn_pixmap, QString description, QString pixmap_path,
          EHorizontalMoveDirection horizontal_direction = EHorizontalMoveDirection::right,
          EVerticalMoveDirection vertical_direction = EVerticalMoveDirection::down);
    EHorizontalMoveDirection m_horizontal_direction;
    EVerticalMoveDirection m_vertical_direction;
};

#endif // CTURN_H
