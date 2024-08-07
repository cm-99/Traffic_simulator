#ifndef CPEDESTRIANCROSSING_H
#define CPEDESTRIANCROSSING_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CPedestrianCrossing class is a graphical representation of pedestrian crossing.
 * Pedestrian crossing allows pedestrians to cross the roadway and must be treated separately from normal crossings.
 */
class CPedestrianCrossing : public CRoadElement
{
public:
    static CStationaryMapElement *create_pedestrian_crossing();

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

private:
    CPedestrianCrossing(QPixmap pedestrian_crossing_pixmap, QString description, EMovementPlane movement_plane, QString pixmap_path);
};

#endif // CPEDESTRIANCROSSING_H
