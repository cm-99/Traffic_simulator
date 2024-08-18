#include "ccar.h"
#include <random>

CCar::CCar(QPixmap car_pixmap, QString description, EMovementPlane starting_movement_plane) :
    CRoadUser(ERoadUsers::car, description, starting_movement_plane)
{
    setPixmap(car_pixmap);
}

CRoadUser *CCar::create_car()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);

    int random_number = dist(gen);

    QPixmap car_pixmap;

    switch (random_number){
    case 0:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_green.png");
        break;
    case 1:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_red.png");
        break;
    case 2:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_yellow.png");
        break;
    default:
        car_pixmap = QPixmap(":/graphics/map_elements_graphics/road_users/cars/car_red.png");
    }

    return new CCar(car_pixmap, "Car", EMovementPlane::horizontal);
}

void CCar::move()
{

}
