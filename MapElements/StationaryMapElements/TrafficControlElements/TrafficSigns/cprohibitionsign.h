#ifndef CPROHIBITIONSIGN_H
#define CPROHIBITIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum class EProhibitionSigns {no_entry, stop, left_turn_prohibited, right_turn_prohibited, turning_prohibited, speed_limit};

/**
 * @brief The CProhibitionSign class can be used to create graphical representations of prohibition signs included in CProhibitionSign.
 */
class CProhibitionSign : public CTrafficSign
{
public:
    EProhibitionSigns get_sign_type() {return m_sign_type;}
    int get_speed_limit() const {return m_speed_limit;}

    static CStationaryMapElement *create_no_entry_sign();
    static CStationaryMapElement *create_stop_sign();
    static CStationaryMapElement *create_left_turn_prohibited_sign();
    static CStationaryMapElement *create_right_turn_prohibited_sign();
    static CStationaryMapElement *create_turning_prohibited_sign();
    static CStationaryMapElement *create_speed_limit_50_sign();
    static CStationaryMapElement *create_speed_limit_30_sign();

private:
    CProhibitionSign(QPixmap sign_pixmap, EProhibitionSigns sign_type, QString description, int speed_limit);

    EProhibitionSigns m_sign_type;
    int m_speed_limit{-1};
};
#endif // CPROHIBITIONSIGN_H
