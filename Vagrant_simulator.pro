QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Logic/capplicationcontroller.cpp \
    MapElements/RoadUsers/ccar.cpp \
    MapElements/StationaryMapElements/RoadElements/ccrossing.cpp \
    MapElements/StationaryMapElements/RoadElements/croadway.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.cpp \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.cpp \
    MapElements/StationaryMapElements/cfiller.cpp \
    MapElements/StationaryMapElements/RoadElements/cpavement.cpp \
    MapElements/RoadUsers/cpedestrian.cpp \
    MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.cpp \
    MapElements/StationaryMapElements/croadelement.cpp \
    MapElements/StationaryMapElements/ctrafficcontrolelement.cpp \
    MapElements/croaduser.cpp \
    MapElements/cstationarymapelement.cpp \
    UI/chomepage.cpp \
    UI/cmainwindow.cpp \
    UI/cmapcreationpage.cpp \
    UI/cmapselectionpage.cpp \
    UI/iuipage.cpp \
    main.cpp \

HEADERS += \
    Logic/capplicationcontroller.h \
    MapElements/RoadUsers/ccar.h \
    MapElements/StationaryMapElements/RoadElements/ccrossing.h \
    MapElements/StationaryMapElements/RoadElements/croadway.h \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinformationsign.h \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cinjuctionsign.h \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cprohibitionsign.h \
    MapElements/StationaryMapElements/TrafficControlElements/ctrafficlight.h \
    MapElements/StationaryMapElements/TrafficControlElements/ctrafficsign.h \
    MapElements/StationaryMapElements/TrafficControlElements/TrafficSigns/cwarningsign.h \
    MapElements/StationaryMapElements/cfiller.h \
    MapElements/StationaryMapElements/RoadElements/cpavement.h \
    MapElements/RoadUsers/cpedestrian.h \
    MapElements/StationaryMapElements/RoadElements/cpedestriancrossing.h \
    MapElements/StationaryMapElements/croadelement.h \
    MapElements/StationaryMapElements/ctrafficcontrolelement.h \
    MapElements/croaduser.h \
    MapElements/cstationarymapelement.h \
    UI/chomepage.h \
    UI/cmainwindow.h \
    UI/cmapcreationpage.h \
    UI/cmapselectionpage.h \
    UI/iuipage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    graphics.qrc

DISTFILES += \
    map_elements_graphics/filler.png \
    map_elements_graphics/pavement.png
