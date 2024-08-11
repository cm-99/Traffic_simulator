#ifndef CAPPLICATIONCONTROLLER_H
#define CAPPLICATIONCONTROLLER_H

#include <QObject>

#include "Logic/cmapcreationcontroller.h"

class CMainWindow;

/**
 * @brief The CApplicationController class is a main application controller.
 *
 * It is the main point of contact between the UI and the application logic.
 * Its main responsibilities are creation of the appropriate model-view-controller sets and
 * handling the loading and saving of maps, using the classes responsible for this.
 */
class CApplicationController
{
public:
    CApplicationController();
    ~CApplicationController();

    bool delegate_map_loading_into_creator(QString map_file_path);
    bool process_simulation_start_request(QString map_file_path);

    const inline QList<QString> &get_supported_map_file_formats() const {return m_map_creation_controller->get_supported_map_file_formats();}
    CMapCreationController *get_map_creation_controller() const {return m_map_creation_controller;}


private:
    CMainWindow *m_main_window;
    CMapCreationController *m_map_creation_controller;

};

#endif // CAPPLICATIONCONTROLLER_H
