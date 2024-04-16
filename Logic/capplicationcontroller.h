#ifndef CAPPLICATIONCONTROLLER_H
#define CAPPLICATIONCONTROLLER_H

#include <QObject>

#include "MapImportAndExportManagers/imapimportandexportmanager.h"

class CMainWindow;

/**
 * @brief The CApplicationController class is a main application controller.
 *
 * It is the main point of contact between the UI and the application logic.
 * Its main responsibilities are creation of the appropriate model-view-controller sets and
 * handling the loading and saving of maps, using the classes responsible for this.
 */
class CApplicationController: public QObject
{
    Q_OBJECT
public:
    CApplicationController();
    ~CApplicationController();

    QMap<QString, IMapImportAndExportManager*> map_formats_mapped_to_managers;
    const QList<QString> &get_supported_map_file_formats() const;
    void delegate_map_loading(QString map_file_path);
    void delegate_map_saving(const CReadOnlyMap* map_to_save, QString map_file_path);

private:
    CMainWindow *m_main_window;
    QList<QString> m_supported_map_file_formats;
};

#endif // CAPPLICATIONCONTROLLER_H
