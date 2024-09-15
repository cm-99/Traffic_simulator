#include "creadonlymap.h"

CReadOnlyMap::CReadOnlyMap(int scene_width, int scene_height)
{
    setSceneRect(0, 0, scene_width, scene_height);
    m_road_users = new QList<CRoadUser*>;
    m_traffic_lights = new QList<CTrafficLight*>;
    m_stationary_map_elements = new QList<CStationaryMapElement*>;

    m_supported_map_sizes = new QList<QSize>;
    m_supported_map_sizes->append(QSize(1920, 1080));
    m_supported_map_sizes->append(QSize(2560, 1440));
    m_supported_map_sizes->append(QSize(3840, 2160));
}

CReadOnlyMap::~CReadOnlyMap()
{
    m_road_users->clear();
    m_traffic_lights->clear();
    m_stationary_map_elements->clear();
    delete m_road_users;
    delete m_traffic_lights;
    delete m_stationary_map_elements;
    this->clear();
}

