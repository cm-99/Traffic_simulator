#ifndef CPROHIBITIONSIGN_H
#define CPROHIBITIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum EProhibitionSigns {no_entry, stop, left_turn_prohibited, right_turn_prohibited, turning_prohibited, speed_limit};
struct SProhibitionSignType{
    EProhibitionSigns m_type;

    SProhibitionSignType(EProhibitionSigns type){
        m_type = type;
    }

    SProhibitionSignType(QString type){
        int type_number = type.toInt();
        if(type_number > 5){
            throw std::invalid_argument("Provided argument type does not translate to any known EProhibitionSigns enum member");
        }
        else{
            m_type = static_cast<EProhibitionSigns>(type_number);
        }
    }

    QString type_number_as_string(){
        return QString::number(m_type);
    }
};
/**
 * @brief The CProhibitionSign class can be used to create graphical representations of prohibition signs included in CProhibitionSign.
 */
class CProhibitionSign : public CTrafficSign
{
public:
    SProhibitionSignType get_sign_type() {return m_sign_type;}
    int get_speed_limit() const {return m_speed_limit;}

    QString serialize_as_string() override;
    QString serialize_type_as_string() override;
    int get_traffic_sign_type_as_int() override;

    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);
    static CStationaryMapElement *create_no_entry_sign();
    static CStationaryMapElement *create_stop_sign();
    static CStationaryMapElement *create_left_turn_prohibited_sign();
    static CStationaryMapElement *create_right_turn_prohibited_sign();
    static CStationaryMapElement *create_turning_prohibited_sign();
    static CStationaryMapElement *create_speed_limit_50_sign();
    static CStationaryMapElement *create_speed_limit_30_sign();

private:
    CProhibitionSign(QPixmap sign_pixmap, SProhibitionSignType sign_type, QString description, int speed_limit, QString pixmap_path);

    SProhibitionSignType m_sign_type;
    int m_speed_limit{-1};
};
#endif // CPROHIBITIONSIGN_H
