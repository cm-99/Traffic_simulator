#include "ccar.h"
#include <random>

CCar::CCar(QPixmap car_pixmap, QString description, EMovementPlane movement_plane, EHorizontalMoveDirection horizontal_move_direction, EVerticalMoveDirection vertical_move_direction) :
    CRoadUser(ERoadUsers::car, description, movement_plane, horizontal_move_direction, vertical_move_direction)
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

    return new CCar(car_pixmap, "Car", EMovementPlane::horizontal, EHorizontalMoveDirection::right);
}

void CCar::move(CReadOnlyMap *map)
{

}
