#ifndef CPAVEMENT_H
#define CPAVEMENT_H

#include "MapElements/StationaryMapElements/croadelement.h"

/**
 * @brief The CPavement class is a graphical representation of pavement.
 */
class CPavement : public CRoadElement
{
public:
    static CStationaryMapElement *create_pavement();

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

private:
    CPavement(QPixmap pavement_pixmap, QString description, EMovementPlane movement_plane, QString pixmap_path);
};

#endif // CPAVEMENT_H
