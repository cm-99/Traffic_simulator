#ifndef CEXPANDABLEWIDGET_H
#define CEXPANDABLEWIDGET_H

#include <QWidget>

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>

/**
 * Widget that can expand/collapse on title press.
 */
class CExpandableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CExpandableWidget(const QString &title = "", const int animation_duration = 300, QWidget *parent = 0);
    /**
     * Sets layout to be used as expandable section.
     */
    void set_content_layout(QLayout &content_layout);
    /**
     * Updates animations and their heights
     */
    void update_heights();
    void set_title(QString title);

private:
    QGridLayout *m_main_layout;
    QToolButton *m_toggle_button;
    QFrame *m_header_line;
    QParallelAnimationGroup *m_toggle_animation;
    QScrollArea *m_content_area;
    int m_animation_duration;
    int m_collapsed_height;
    bool m_is_expanded = false;

public slots:
    /**
     * Toggles the expandable widget on toggle_button click.
     */
    void slot_toggle(bool checked);
};

#endif // CEXPANDABLEWIDGET_H
