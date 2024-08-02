#ifndef CMAPELEMENTSPLACEMENTWIDGET_H
#define CMAPELEMENTSPLACEMENTWIDGET_H

#include "MapElements/StationaryMapElements/croadelement.h"
#include "MapElements/StationaryMapElements/ctrafficcontrolelement.h"
#include "UI/cexpandablewidget.h"
#include "qpushbutton.h"
#include <QWidget>
#include <QFormLayout>

class CMapCreationPage;

class CMapElementsPlacementWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMapElementsPlacementWidget(CMapCreationPage *map_creation_page, QWidget *parent = nullptr);
    ~CMapElementsPlacementWidget();

private:
    CMapCreationPage *m_map_creation_page;
    QVBoxLayout m_main_layout;

    QGridLayout m_pavement_elements_buttons_layout;
    QGridLayout m_roadway_elements_buttons_layout;
    QGridLayout m_traffic_lights_buttons_layout;
    QGridLayout m_warning_signs_buttons_layout;
    QGridLayout m_information_signs_buttons_layout;
    QGridLayout m_prohibition_signs_buttons_layout;
    QGridLayout m_injuction_signs_buttons_layout;

    QMap<QPushButton*, CStationaryMapElement *(*)(void)> m_buttons_mapped_to_creators;

    void add_stationary_map_element_creation_button(CStationaryMapElement *(*creation_func)(void));
    CExpandableWidget* create_nested_expandable_widget(QFrame *parent_frame, QString title, QLayout& widget_layout);
    void add_button_to_grid_layout(QPushButton *button, QGridLayout *grid_layout);
    void add_map_elements_creation_buttons();
};

#endif // CMAPELEMENTSPLACEMENTWIDGET_H
