#ifndef IMAPIMPORTANDEXPORTMANAGER_H
#define IMAPIMPORTANDEXPORTMANAGER_H

#include <QString>
#include "Logic/MapModels/ceditablemap.h"

/**
 * @brief The IMapImportAndExportManager is an interface for classes used to import and export map.
 */
class IMapImportAndExportManager
{
public:
    /**
     * @brief get_supported_format - returns supported map file format as string.
     * @return
     */
    virtual const QString& get_supported_format() = 0;
    virtual ~IMapImportAndExportManager() {};

    /**
     * @brief load_map - loads map from file
     * @param map_file_path
     * @return returns loaded map if successful; otherwise returns nullptr
     */
    virtual CEditableMap* load_map(QString map_file_path) = 0;
    /**
     * @brief save_map - saves map to file
     * @param map_to_save - pointer to a map that is to be saved
     * @param map_file_path
     * @return returns true if saving was successful; otherwise returns fale
     */
    virtual bool save_map(const CReadOnlyMap *map_to_save, QString map_file_path) = 0;
};

#endif // IMAPIMPORTANDEXPORTMANAGER_H
