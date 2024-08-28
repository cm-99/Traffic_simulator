#ifndef CCROSSING_H
#define CCROSSING_H

#include "MapElements/StationaryMapElements/RoadElements/croadwayelement.h"

/**
 * @brief The CCrossing class is a graphical representation of crossing/intersection.
 */
class CCrossing : public CRoadwayElement
{
public:
    static CStationaryMapElement *create_single_carriageway_car_crossing();
    static CStationaryMapElement *create_double_carriageway_car_crossing();

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);
    inline void set_crossing_valid_sides(QVector<int> valid_sides) {m_valid_sides = valid_sides;}
    inline QVector<int> get_crossing_valid_sides() const {return m_valid_sides;}

private:
    CCrossing(int carriageways_number, int lanes_number, QPixmap crossing_pixmap, EPermittedRoadUsers permitted_road_users, QString description, QString pixmap_path,
              EMovementPlane movement_plane = EMovementPlane::any);
    /**
     * @brief m_valid_sides - valid sides of a crossing, meaning, either roadway or pedestrian crossing
     * Theoretically road user should be able to "see" which sides are valid, but with lots of road users it can get unnecessarily long
     */
    QVector<int> m_valid_sides;
};

#endif // CCROSSING_H
