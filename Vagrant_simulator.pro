QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Logic/capplicationcontroller.cpp \
    MapElements/croaduser.cpp \
    UI/chomepage.cpp \
    UI/cmainwindow.cpp \
    UI/cmapcreationpage.cpp \
    UI/cmapselectionpage.cpp \
    UI/iuipage.cpp \
    main.cpp \

HEADERS += \
    Logic/capplicationcontroller.h \
    MapElements/croaduser.h \
    UI/chomepage.h \
    UI/cmainwindow.h \
    UI/cmapcreationpage.h \
    UI/cmapselectionpage.h \
    UI/iuipage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
