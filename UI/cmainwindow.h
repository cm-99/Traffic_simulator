#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

#include "chomepage.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

private:
    CHomePage home_page;

private slots:
    void slot_configure_simulation();
    void slot_show_map_creation_page();
    void slot_show_map_selection_page();
    void slot_show_map_file_dialog();
};

#endif // CMAINWINDOW_H
