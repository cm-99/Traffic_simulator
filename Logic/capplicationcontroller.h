#ifndef CAPPLICATIONCONTROLLER_H
#define CAPPLICATIONCONTROLLER_H

#include "Logic/cmapcreationcontroller.h"
#include "Logic/csimulationcontroller.h"

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

    /**
     * @brief Delegates map loading into map creator to the map creation controller.
     * @param map_file_path - path to the file which contains map to be loaded
     * @return returns true if operation was successful; otherwise returns false
     */
    bool delegate_map_loading_into_creator(QString map_file_path);
    /**
     * @brief Processes simulation start request. Delegates map loading using provided path to the map file.
     * @param map_file_path - path to the map file on which simulation should take place
     * @return returns true if operation was successful; otherwise returns false
     */
    bool process_simulation_start_request(QString map_file_path);
    /**
     * @brief Returns map file formats supported by classes implementing IMapImportAndExportManager interface.
     * @return returns list of supported formats
     */
    const inline QList<QString> &get_supported_map_file_formats() const {return m_map_creation_controller->get_supported_map_file_formats();}
    CMapCreationController *get_map_creation_controller() const {return m_map_creation_controller;}
    CSimulationController *get_simulation_controller() const {return m_simulation_controller;}

private:
    CMainWindow *m_main_window;
    CMapCreationController *m_map_creation_controller;
    CSimulationController *m_simulation_controller{nullptr};
};

#endif // CAPPLICATIONCONTROLLER_H
