#ifndef CROADUSERSPLACEMENTWIDGET_H
#define CROADUSERSPLACEMENTWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include "MapElements/croaduser.h"
#include <QGridLayout>

class CSimulationPage;

class CRoadUsersPlacementWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRoadUsersPlacementWidget(CSimulationPage *simulation_page, QWidget *parent = nullptr);
    ~CRoadUsersPlacementWidget();

private:
    CSimulationPage *m_simulation_page;
    QGridLayout m_cars_buttons_layout;
    QGridLayout m_pedestrians_buttons_layout;

    QMap<QPushButton*, CRoadUser *(*)(void)> m_buttons_mapped_to_creators;

    void add_road_users_creation_buttons();
    void add_road_user_creation_button(CRoadUser *(*creation_func)(void));
    void add_button_to_grid_layout(QPushButton *button, QGridLayout *grid_layout);
};

#endif // CROADUSERSPLACEMENTWIDGET_H
