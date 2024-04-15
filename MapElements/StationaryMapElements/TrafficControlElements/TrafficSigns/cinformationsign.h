#ifndef CINFORMATIONSIGN_H
#define CINFORMATIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum class EInformationSigns{one_way_road, pedestrian_crossing};

/**
 * @brief The CInformationSign class can be used to create graphical representations of warning signs included in EInformationSigns.
 */
class CInformationSign : public CTrafficSign
{
public:
    CInformationSign(QPixmap sign_pixmap, EInformationSigns sign_type);
    EInformationSigns get_sign_type() {return m_sign_type;}

private:
    EInformationSigns m_sign_type;
};

#endif // CINFORMATIONSIGN_H
