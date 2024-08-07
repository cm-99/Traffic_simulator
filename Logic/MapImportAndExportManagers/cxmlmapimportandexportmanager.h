#ifndef CXMLMAPIMPORTANDEXPORTMANAGER_H
#define CXMLMAPIMPORTANDEXPORTMANAGER_H

#include "imapimportandexportmanager.h"

class CXMLMapImportAndExportManager : public IMapImportAndExportManager
{
public:
    CXMLMapImportAndExportManager();
    ~CXMLMapImportAndExportManager() {};

    const QString& get_supported_format() {return m_supported_format;}
    CEditableMap *load_map(QString map_file_path);
    void save_map(const CReadOnlyMap *map_to_save, QString map_file_path);

private:
    const QString m_supported_format{"*.xml"};
};

#endif // CXMLMAPIMPORTANDEXPORTMANAGER_H
