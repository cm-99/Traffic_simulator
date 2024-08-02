#ifndef CWARNINGSIGN_H
#define CWARNINGSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum class EWarningSigns {give_way, equivalent_intersection};

/**
 * @brief The CWarningSign class can be used to create graphical representations of warning signs included in EWarningSign.
 */
class CWarningSign : public CTrafficSign
{
public:
    EWarningSigns get_sign_type() {return m_sign_type;}

    static CStationaryMapElement *create_give_way_sign();
    static CStationaryMapElement *create_equivalent_intersection_sign();

private:
    CWarningSign(QPixmap sign_pixmap, EWarningSigns sign_type, QString description);
    EWarningSigns m_sign_type;
};

#endif // CWARNINGSIGN_H
