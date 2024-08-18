#include "cpedestrian.h"

CPedestrian::CPedestrian(QVector<QPixmap> pedestrian_states_pixmaps, QString description, EMovementPlane starting_movement_plane) :
    CRoadUser(ERoadUsers::pedestrian, description, starting_movement_plane)
{
    setPixmap(pedestrian_states_pixmaps[0]);
}

CRoadUser *CPedestrian::create_pedestrian()
{
    return new CPedestrian(QVector<QPixmap>{QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right_still.png"),
                                            QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right.png"),
                                            QPixmap(":/graphics/map_elements_graphics/road_users/pedestrians/pedestrian_right_2.png")},
                           "Pedestrian", EMovementPlane::horizontal);
}

void CPedestrian::move()
{

}

