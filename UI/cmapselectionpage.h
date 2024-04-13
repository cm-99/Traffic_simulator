#ifndef CMAPSELECTIONPAGE_H
#define CMAPSELECTIONPAGE_H

#include <QBoxLayout>
#include <QPushButton>

#include "iuipage.h"

/**
 * @brief The CMapSelectionPage class is an UI page providing prepared simulation maps for the user to choose from.
 * After choosing a map the user can either move to editing it or start simulation on the chosen map.
 */
class CMapSelectionPage : public IUIPage
{
    Q_OBJECT
public:
    explicit CMapSelectionPage(QWidget *parent = nullptr);

    QVBoxLayout m_main_layout;
    QGridLayout m_map_miniatures_layout;
    QHBoxLayout m_buttons_layout;

    QPushButton m_button_edit_map;
    QPushButton m_button_start_simulation;
};

#endif // CMAPSELECTIONPAGE_H
