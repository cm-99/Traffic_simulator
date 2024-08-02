#include <QPropertyAnimation>

#include "cexpandablewidget.h"

CExpandableWidget::CExpandableWidget(const QString &title, const int animation_duration, QWidget *parent) :
    QWidget{parent},
    m_main_layout(new QGridLayout(this)),
    m_toggle_button(new QToolButton(this)),
    m_header_line(new QFrame(this)),
    m_toggle_animation(new QParallelAnimationGroup(this)),
    m_content_area(new QScrollArea(this)),
    m_animation_duration(animation_duration)
{
    m_toggle_button->setStyleSheet("QToolButton { border: none; }");
    m_toggle_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggle_button->setArrowType(Qt::ArrowType::RightArrow);
    m_toggle_button->setText(title);
    m_toggle_button->setCheckable(true);
    m_toggle_button->setChecked(false);

    m_header_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_content_area->setStyleSheet("QScrollArea { border: none; }");
    m_content_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //Start out collapsed
    m_content_area->setMaximumHeight(0);
    m_content_area->setMinimumHeight(0);

    //Let the entire widget grow and shrink with its content
    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(m_content_area, "maximumHeight"));

    m_main_layout->setVerticalSpacing(0);
    m_main_layout->setContentsMargins(0, 0, 0, 0);

    int row = 0;
    m_main_layout->addWidget(m_toggle_button, row, 0, 1, 1, Qt::AlignLeft);
    m_main_layout->addWidget(m_header_line, row++, 2, 1, 1);
    m_main_layout->addWidget(m_content_area, row, 0, 1, 3);
    setLayout(m_main_layout);

    connect(m_toggle_button, &QToolButton::toggled, this, &CExpandableWidget::slot_toggle);
}

void CExpandableWidget::set_content_layout(QLayout &content_layout)
{
    delete m_content_area->layout();
    m_content_area->setLayout(&content_layout);
    m_collapsed_height = sizeHint().height() - m_content_area->maximumHeight();

    update_heights();
}

void CExpandableWidget::set_title(QString title)
{
    m_toggle_button -> setText(title);
}

void CExpandableWidget::update_heights()
{
    int content_height = m_content_area->layout()->sizeHint().height();

    for (int i = 0; i < m_toggle_animation->animationCount() - 1; ++i)
    {
        QPropertyAnimation* section_animation = static_cast<QPropertyAnimation *>(m_toggle_animation->animationAt(i));
        section_animation ->setDuration(m_animation_duration);
        section_animation ->setStartValue(m_collapsed_height);
        section_animation ->setEndValue(m_collapsed_height + content_height);
    }

    QPropertyAnimation* content_animation =
        static_cast<QPropertyAnimation *>(m_toggle_animation->animationAt(m_toggle_animation->animationCount() - 1));
    content_animation->setDuration(m_animation_duration);
    content_animation->setStartValue(0);
    content_animation->setEndValue(content_height);

    m_toggle_animation->setDirection(m_is_expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_toggle_animation->start();
}

void CExpandableWidget::slot_toggle(bool checked)
{
    m_toggle_button->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    m_toggle_animation->setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_toggle_animation->start();

    this->m_is_expanded = checked;
}
