#ifndef CTRAFFICSIGN_H
#define CTRAFFICSIGN_H

#include "MapElements/StationaryMapElements/ctrafficcontrolelement.h"

enum ETrafficSignCategory {warning_sign = 0, injuction_sign = 1, prohibition_sign = 2, information_sign = 3};

/**
 * @brief The CTrafficSign class is a base class for all the classes representing
 * different categories of traffic signs included in ETrafficSignCategory.
 *
 * TODO: Try to use template class for child classes? But casting to specific type would still be needed...
 */
class CTrafficSign : public CTrafficControlElement
{
public:
    CTrafficSign(ETrafficSignCategory traffic_sign_category, QString description, QString pixmap_path) :
        CTrafficControlElement(ETrafficControlElementType::traffic_sign, description, pixmap_path),
        m_traffic_sign_category(traffic_sign_category){}

    virtual QString serialize_as_string()=0;
    virtual QString serialize_type_as_string()=0;

    virtual int get_traffic_sign_type_as_int()=0;
    inline ETrafficSignCategory get_traffic_sign_category() const {return m_traffic_sign_category;}

private:
    ETrafficSignCategory m_traffic_sign_category;
};

#endif // CTRAFFICSIGN_H
