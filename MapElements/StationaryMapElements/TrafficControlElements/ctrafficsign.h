#ifndef CTRAFFICSIGN_H
#define CTRAFFICSIGN_H

#include "MapElements/StationaryMapElements/ctrafficcontrolelement.h"

enum class ETrafficSignCategory {warning_sign, injuction_sign, prohibition_sign, information_sign};

/**
 * @brief The CTrafficSign class is a base class for all the classes representing
 * different categories of traffic signs included in ETrafficSignCategory.
 *
 * TODO: Try to use template class for child classes? But casting to specific type would still be needed...
 */
class CTrafficSign : public CTrafficControlElement
{
public:
    CTrafficSign(ETrafficSignCategory traffic_sign_category, QString description) :
        CTrafficControlElement(ETrafficControlElementType::traffic_sign, description),
        m_traffic_sign_category(traffic_sign_category){}

    ETrafficSignCategory get_traffic_sign_category() const {return m_traffic_sign_category;}
private:
    ETrafficSignCategory m_traffic_sign_category;
};

#endif // CTRAFFICSIGN_H
