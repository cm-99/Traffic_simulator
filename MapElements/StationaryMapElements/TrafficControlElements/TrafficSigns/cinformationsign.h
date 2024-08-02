#ifndef CINFORMATIONSIGN_H
#define CINFORMATIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum class EInformationSigns{one_way_road, pedestrian_crossing, priority_road};

/**
 * @brief The CInformationSign class can be used to create graphical representations of warning signs included in EInformationSigns.
 */
class CInformationSign : public CTrafficSign
{
public:
    EInformationSigns get_sign_type() {return m_sign_type;}

    static CStationaryMapElement *create_priority_road_sign();
    static CStationaryMapElement *create_pedestrian_crossing_sign();
    static CStationaryMapElement *create_one_wady_road_sign();

private:
    CInformationSign(QPixmap sign_pixmap, EInformationSigns sign_type, QString description);
    EInformationSigns m_sign_type;
};

#endif // CINFORMATIONSIGN_H
