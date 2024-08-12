#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStackedWidget>

#include "Logic/capplicationcontroller.h"
#include "cmapcreationpage.h"
#include "chomepage.h"
#include "cmapselectionpage.h"
#include "UI/csimulationconfigurationdialog.h"

/**
 * @brief The CMainWindow class is a parent and container of other UI classes.
 * It handles UI pages switching and communication of UI with main application logic in the form of CApplicationController object.
 */
class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = nullptr, CApplicationController *application_controller = nullptr);
    ~CMainWindow();
    void relay_simulation_start_request(QString map_file_path);
    SSimulationConfiguration get_simulation_configuration_from_user();
    void show_simulation_page();

private:
    CApplicationController *m_application_controller;

    QStackedWidget stacked_widget;
    CHomePage m_home_page;
    CMapSelectionPage m_map_selection_page;
    CMapCreationPage m_map_creation_page;

    int previous_page_index{0};
    QPushButton m_back_button;
    QPushButton m_home_page_button;

public slots:
    void slot_open_map_in_creator(QString map_file_path);

private slots:
    /**
     * @brief slot_show_map_creation_page switches current central widget of CMainWindow object to m_map_creation_page.
     */
    void slot_show_map_creation_page();
    /**
     * @brief slot_show_map_selection_page switches current central widget of CMainWindow object to m_map_selection_page.
     */
    void slot_show_map_selection_page();
    /**
     * @brief slot_process_map_loading_request provides the user ability to choose a map file to be loaded from file system window
     * and forwards map loading to m_application_controller.
     */
    void slot_process_map_loading_request();
    void slot_switch_to_previous_page();
    void slot_back_to_home_page();
};

#endif // CMAINWINDOW_H
