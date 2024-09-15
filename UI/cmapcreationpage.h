#ifndef CMAPCREATIONPAGE_H
#define CMAPCREATIONPAGE_H

#include "UI/cmapelementsplacementwidget.h"
#include "iuipage.h"
#include "qboxlayout.h"
#include "Logic/cmapcreationcontroller.h"
#include <QWidget>

/**
 * @brief The CMapCreationPage class is an UI page providing map creation and edition functionality.
 */
class CMapCreationPage : public IUIPage
{
    Q_OBJECT
public:
    explicit CMapCreationPage(CMapCreationController *map_creation_controller, QWidget *parent = nullptr);
    void restore_to_default() override;

    void relay_creator_button_clicked(CStationaryMapElement *(*creation_func)(void));

private:
    QVBoxLayout m_main_layout;
    CMapCreationController* m_map_creation_controller;
    CMapElementsPlacementWidget* m_map_elements_placement_widget;

private slots:
    void relay_map_resize_request();
    void relay_map_saving_request();
    void relay_simulation_start_request();
    void process_map_clearing_request();
};

#endif // CMAPCREATIONPAGE_H
