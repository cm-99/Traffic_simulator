#ifndef CXMLMAPIMPORTANDEXPORTMANAGER_H
#define CXMLMAPIMPORTANDEXPORTMANAGER_H

#include "imapimportandexportmanager.h"

/**
 * @brief The CXMLMapImportAndExportManager class is used to write and read the map in xml format
 */
class CXMLMapImportAndExportManager : public IMapImportAndExportManager
{
public:
    CXMLMapImportAndExportManager();
    ~CXMLMapImportAndExportManager() {};

    /**
     * @brief get_supported_format - returns supported map file format as string - *.xml
     * @return
     */
    const QString& get_supported_format() override {return m_supported_format;}

    /**
     * @brief load_map - loads map from file
     * @param map_file_path
     * @return returns loaded map if successful; otherwise returns nullptr
     */
    CEditableMap* load_map(QString map_file_path) override;

    /**
     * @brief save_map - saves map to file
     * @param map_to_save - pointer to a map that is to be saved
     * @param map_file_path
     * @return returns true if saving was successful; otherwise returns fale
     */
    bool save_map(const CReadOnlyMap *map_to_save, QString map_file_path) override;

private:
    const QString m_supported_format{"*.xml"};
};

#endif // CXMLMAPIMPORTANDEXPORTMANAGER_H
