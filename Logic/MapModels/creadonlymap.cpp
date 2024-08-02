#include "creadonlymap.h"

CReadOnlyMap::CReadOnlyMap()
{
    // Scene size - hard-coded for now :/
    setSceneRect(0, 0, 1920, 1080);
    m_road_users = new QList<CRoadUser*>;
    m_traffic_lights = new QList<CTrafficLight*>;
}

CReadOnlyMap::~CReadOnlyMap()
{
    m_road_users->clear();
    delete m_road_users;
    m_traffic_lights->clear();
    delete m_traffic_lights;
    this->clear();
}

