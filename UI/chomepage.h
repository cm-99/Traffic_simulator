#ifndef CHOMEPAGE_H
#define CHOMEPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>

class CHomePage : public QWidget
{
    Q_OBJECT
public:
    explicit CHomePage(QWidget *parent = nullptr);

private:
    QPushButton m_button_create_new_map;
    QPushButton m_button_load_map_file;
    QPushButton m_button_select_map;

    QVBoxLayout m_buttons_layout;
};

#endif // CHOMEPAGE_H
