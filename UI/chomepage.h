#ifndef CHOMEPAGE_H
#define CHOMEPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>

#include "iuipage.h"

/**
 * @brief The CHomePage class is a starting UI page of the application.
 *
 * It serves as a selection point for further actions in the context of creating or selecting a simulation map.
 * CHomePage inherits IUIPage only in order to standarize UI pages.
 */
class CHomePage : public IUIPage
{
    Q_OBJECT
public:
    explicit CHomePage(QWidget *parent = nullptr);
    /**
     * @brief restore_to_default in the case of this class is not implemented,
     * because there are no changes in the state of the page possible.
     */
    void restore_to_default() override {};

private:
    QPushButton m_button_create_new_map;
    QPushButton m_button_load_map_file;
    QPushButton m_button_select_map;

    QVBoxLayout m_buttons_layout;

signals:
    /**
     * @brief signal_map_creator_requested is connected to m_button_create_new_map signal clicked
     *
     * It is used to forward map creation request and should be connected to a slot handling UI page switching to CMapCreationPage object.
     */
    void signal_map_creator_requested();
    /**
     * @brief signal_map_loading_requested is connected to m_button_load_map_file signal clicked
     *
     * It is used to forward map loading request and should be connected to a slot handling such functionality.
     */
    void signal_map_loading_requested();
    /**
     * @brief signal_map_selection_requested is connected to m_button_select_map signal clicked
     *
     * It is used to forward map selection request and should be connected to a slot handling UI page switching to CMapSelectionPage object.
     */
    void signal_map_selection_requested();
};

#endif // CHOMEPAGE_H
