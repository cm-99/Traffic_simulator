#ifndef CINFORMATIONSIGN_H
#define CINFORMATIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum EInformationSigns{one_way_road, pedestrian_crossing_sign, priority_road, built_up_area_sign, non_built_up_area_sign};
struct SInformationSignType{
    EInformationSigns m_type;

    SInformationSignType(EInformationSigns type){
        m_type = type;
    }

    SInformationSignType(QString type){
        int type_number = type.toInt();
        if(type_number > 2){
            throw std::invalid_argument("Provided argument type does not translate to any known EInformationSigns enum member");
        }
        else{
            m_type = static_cast<EInformationSigns>(type_number);
        }
    }

    QString type_number_as_string(){
        return QString::number(m_type);
    }
};

/**
 * @brief The CInformationSign class can be used to create graphical representations of warning signs included in EInformationSigns.
 */
class CInformationSign : public CTrafficSign
{
public:
    SInformationSignType get_sign_type() {return m_sign_type;}

    virtual QString serialize_as_string();
    virtual QString serialize_type_as_string();
    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

    static CStationaryMapElement *create_priority_road_sign();
    static CStationaryMapElement *create_pedestrian_crossing_sign();
    static CStationaryMapElement *create_one_wady_road_sign();
    static CStationaryMapElement *create_built_up_area_sign();
    static CStationaryMapElement *create_non_built_up_area_sign();

private:
    CInformationSign(QPixmap sign_pixmap, SInformationSignType sign_type, QString description, QString pixmap_path);
    SInformationSignType m_sign_type;
};

#endif // CINFORMATIONSIGN_H
