#include <QFile>
#include <QFileInfo>

#include "capplicationcontroller.h"
#include "UI/cmainwindow.h"
#include "MapImportAndExportManagers/cxmlmapimportandexportmanager.h"

CApplicationController::CApplicationController()
{
    m_main_window = new CMainWindow(nullptr, this);
    m_main_window->show();

    auto *xml_map_manager = new CXMLMapImportAndExportManager();
    map_formats_mapped_to_managers.insert(xml_map_manager->get_supported_format(), xml_map_manager);
}

CApplicationController::~CApplicationController()
{
    // for(auto *manager: map_formats_mapped_to_managers){
    //     delete manager;
    // }
    delete m_main_window;
}

const QList<QString>& CApplicationController::get_supported_map_file_formats() const
{
    return m_supported_map_file_formats;
}

void CApplicationController::delegate_map_loading(QString map_file_path)
{
    QFileInfo map_file_info{map_file_path};
    QString map_file_format = "*." + map_file_info.completeSuffix();

    auto *map_export_manager = map_formats_mapped_to_managers.value(map_file_format);
    auto *map_model = map_export_manager ->load_map(map_file_path);
}

void CApplicationController::delegate_map_saving(const CReadOnlyMap* map_to_save, QString map_file_path)
{
    QFileInfo map_file_info{map_file_path};
    QString map_file_format = "*." + map_file_info.completeSuffix();

    auto *map_import_manager = map_formats_mapped_to_managers.value(map_file_format);
    map_import_manager -> save_map(map_to_save, map_file_path);
}

