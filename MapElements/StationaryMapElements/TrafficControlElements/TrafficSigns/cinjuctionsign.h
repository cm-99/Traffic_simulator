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
    CInjuctionSign(QPixmap sign_pixmap, EInjuctionSigns sign_type);
    EInjuctionSigns get_sign_type() {return m_sign_type;}

private:
    EInjuctionSigns m_sign_type;
};

#endif // CINJUCTIONSIGN_H
