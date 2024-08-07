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

private:
    CCrossing(int carriageways_number, int lanes_number, QPixmap crossing_pixmap, EPermittedRoadUsers permitted_road_users, QString description, QString pixmap_path,
              EMovementPlane movement_plane = EMovementPlane::any);
};

#endif // CCROSSING_H
