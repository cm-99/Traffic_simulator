#ifndef CWARNINGSIGN_H
#define CWARNINGSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum EWarningSigns {give_way, equivalent_intersection};
struct SWarningSignType{
    EWarningSigns m_type;

    SWarningSignType(EWarningSigns type){
        m_type = type;
    }

    SWarningSignType(QString type){
        int type_number = type.toInt();
        if(type_number > 1){
            throw std::invalid_argument("Provided argument type does not translate to any known EWarningSigns enum member");
        }
        else{
            m_type = static_cast<EWarningSigns>(type_number);
        }
    }

    QString type_number_as_string(){
        return QString::number(m_type);
    }
};
/**
 * @brief The CWarningSign class can be used to create graphical representations of warning signs included in EWarningSign.
 */
class CWarningSign : public CTrafficSign
{
public:
    SWarningSignType get_sign_type() {return m_sign_type;}

    QString serialize_as_string() override;
    QString serialize_type_as_string() override;
    int get_traffic_sign_type_as_int() override;

    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);
    static CStationaryMapElement *create_give_way_sign();
    static CStationaryMapElement *create_equivalent_intersection_sign();

private:
    CWarningSign(QPixmap sign_pixmap, SWarningSignType sign_type, QString description, QString pixmap_path);
    SWarningSignType m_sign_type;
};

#endif // CWARNINGSIGN_H
