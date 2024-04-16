#ifndef IMAPIMPORTANDEXPORTMANAGER_H
#define IMAPIMPORTANDEXPORTMANAGER_H

#include <QString>
#include "Logic/MapModels/ceditablemap.h"

class IMapImportAndExportManager
{
public:
    virtual const QString& get_supported_format() = 0;
    virtual ~IMapImportAndExportManager() {};
    //TODO: wrap in unique_ptr
    virtual CEditableMap *load_map(QString map_file_path) = 0;
    virtual void save_map(const CReadOnlyMap *map_to_save, QString map_file_path) = 0;
};

#endif // IMAPIMPORTANDEXPORTMANAGER_H
