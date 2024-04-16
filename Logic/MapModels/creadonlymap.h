#ifndef CREADONLYMAP_H
#define CREADONLYMAP_H

#include <QGraphicsScene>

#include "MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h"
#include "MapElements/croaduser.h"
/**
 * @brief The CReadOnlyMap class is base class for all map models present in the application.
 * Functions as a basic container of map elements.
 */
class CReadOnlyMap: public QGraphicsScene
{
public:
    CReadOnlyMap();
    ~CReadOnlyMap();
    QList<CRoadUser *> *get_road_users() const {return m_road_users;}
    QList<CStationaryMapElement *> *get_stationary_map_elements() const {return m_stationary_map_elements;}
    QList<CStationaryMapElement *> *get_traffic_lights() const {return m_stationary_map_elements;}

protected:
    // Needed for quick access to the objects which depend on simulation speed/timers
    QList<CRoadUser*> *m_road_users;
    QList<CTrafficLight*> *m_traffic_lights;

    // Not sure yet if needed, I hope it made it to the class diagram for a reason...
    QList<CStationaryMapElement*> *m_stationary_map_elements;
};

#endif // CREADONLYMAP_H
