#ifndef CINJUCTIONSIGN_H
#define CINJUCTIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum class EInjuctionSigns{drive_straight, turn_left, turn_right};

/**
 * @brief The CInjuctionSign class can be used to create graphical representations of warning signs included in EInjuctionSigns.
 */
class CInjuctionSign : public CTrafficSign
{
public:
    EInjuctionSigns get_sign_type() {return m_sign_type;}

    static CStationaryMapElement *create_drive_straight_sign();
    static CStationaryMapElement *create_turn_left_sign();
    static CStationaryMapElement *create_turn_right_sign();

private:
    CInjuctionSign(QPixmap sign_pixmap, EInjuctionSigns sign_type, QString description);
    EInjuctionSigns m_sign_type;
};

#endif // CINJUCTIONSIGN_H
