#ifndef IUIPAGE_H
#define IUIPAGE_H

#include <QWidget>

/**
 * @brief The IUIPage is a base class for all the classes meant to be used as main widgets in CMainWindow,
 * which will also be referred to as UI pages.
 */
class IUIPage : public QWidget
{
    Q_OBJECT
public:
    virtual ~IUIPage() {};
    /**
     * @brief restore_to_default is a method to be implemented as a mechanism of restoration to default page condition
     */
    virtual void restore_to_default() {};
};

#endif // IUIPAGE_H
