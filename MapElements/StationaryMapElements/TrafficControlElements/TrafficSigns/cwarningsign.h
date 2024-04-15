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
    CWarningSign(QPixmap sign_pixmap, EWarningSigns sign_type);
    EWarningSigns get_sign_type() {return m_sign_type;}

private:
    EWarningSigns m_sign_type;
};

#endif // CWARNINGSIGN_H
