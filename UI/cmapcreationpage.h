#ifndef CMAPCREATIONPAGE_H
#define CMAPCREATIONPAGE_H

#include "iuipage.h"
#include <QWidget>

/**
 * @brief The CMapCreationPage class is an UI page providing map creation and edition functionality.
 */
class CMapCreationPage : public IUIPage
{
    Q_OBJECT
public:
    explicit CMapCreationPage(QWidget *parent = nullptr);

private:

signals:
};

#endif // CMAPCREATIONPAGE_H
