#ifndef CINJUCTIONSIGN_H
#define CINJUCTIONSIGN_H

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h"

enum EInjuctionSigns{drive_straight, turn_left, turn_right};
struct SInjuctionSignType{
    EInjuctionSigns m_type;

    SInjuctionSignType(EInjuctionSigns type){
        m_type = type;
    }

    SInjuctionSignType(QString type){
        int type_number = type.toInt();
        if(type_number > 2){
            throw std::invalid_argument("Provided argument type does not translate to any known EInjuctionSigns enum member");
        }
        else{
            m_type = static_cast<EInjuctionSigns>(type_number);
        }
    }

    QString type_number_as_string(){
        return QString::number(m_type);
    }
};
/**
 * @brief The CInjuctionSign class can be used to create graphical representations of warning signs included in EInjuctionSigns.
 */
class CInjuctionSign : public CTrafficSign
{
public:
    SInjuctionSignType get_sign_type() {return m_sign_type;}

    QString serialize_as_string() override;
    QString serialize_type_as_string() override;
    int get_traffic_sign_type_as_int() override;

    static CStationaryMapElement *deserialize_from_string(QString item_serialized_to_string);

    static CStationaryMapElement *create_drive_straight_sign();
    static CStationaryMapElement *create_turn_left_sign();
    static CStationaryMapElement *create_turn_right_sign();

private:
    CInjuctionSign(QPixmap sign_pixmap, SInjuctionSignType sign_type, QString description, QString pixmap_path);
    SInjuctionSignType m_sign_type;
};

#endif // CINJUCTIONSIGN_H
