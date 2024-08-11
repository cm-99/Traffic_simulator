#ifndef CMAPSELECTIONPAGE_H
#define CMAPSELECTIONPAGE_H

#include <QBoxLayout>
#include <QPushButton>

#include "iuipage.h"

class CMainWindow;

/**
 * @brief The CMapSelectionPage class is an UI page providing prepared simulation maps for the user to choose from.
 * After choosing a map the user can either move to editing it or start simulation on the chosen map.
 */
class CMapSelectionPage : public IUIPage
{
    Q_OBJECT
public:
    explicit CMapSelectionPage(CMainWindow *parent);
    void restore_to_default() override;

private:
    CMainWindow *m_main_window;

    QVBoxLayout m_main_layout;
    QHBoxLayout m_map_miniatures_layout;
    QHBoxLayout m_buttons_layout;

    QString m_current_map_file_path;
    QPushButton m_button_edit_map;
    QPushButton m_button_configure_simulation;

signals:
    void signal_request_simulation_configuration();
};

#endif // CMAPSELECTIONPAGE_H
