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
    CProhibitionSign(QPixmap sign_pixmap, EProhibitionSigns sign_type);

    int get_speed_limit() const {return m_speed_limit;}
    EProhibitionSigns get_sign_type() {return m_sign_type;}

    void set_speed_limit(int speed_limit) {m_speed_limit = speed_limit;}

private:
    EProhibitionSigns m_sign_type;
    int m_speed_limit{-1};
};
#endif // CPROHIBITIONSIGN_H
