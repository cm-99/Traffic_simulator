QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Logic/MapImportAndExportManagers/cxmlmapimportandexportmanager.cpp \
    Logic/MapModels/ceditablemap.cpp \
    Logic/MapViews/cmapcreationview.cpp \
    Logic/MapViews/csimulationview.cpp \
    Logic/capplicationcontroller.cpp \
    Logic/cbasecontroller.cpp \
    Logic/MapModels/creadonlymap.cpp \
    Logic/MapViews/cbaseview.cpp \
    Logic/cmapcreationcontroller.cpp \
    Logic/csimulationcontroller.cpp \
    Logic/MapImportAndExportManagers/imapimportandexportmanager.cpp \
    MapElements/RoadUsers/ccar.cpp \
    MapElements/StationaryMapElements/RoadElements/ccrossing.cpp \
    MapElements/StationaryMapElements/RoadElements/croadway.cpp \
    MapElements/StationaryMapElements/RoadElements/croadwayelement.cpp \
    MapElements/StationaryMapElements/RoadElements/cturn.cpp \
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
    UI/cexpandablewidget.cpp \
    UI/chomepage.cpp \
    UI/cmainwindow.cpp \
    UI/cmapcreationpage.cpp \
    UI/cmapelementsplacementwidget.cpp \
    UI/cmapselectionpage.cpp \
    UI/croadusersparametersinputwidget.cpp \
    UI/croadusersplacementwidget.cpp \
    UI/csimulationconfigurationdialog.cpp \
    UI/csimulationpage.cpp \
    UI/ctrafficlightsdurationinputwidget.cpp \
    UI/iuipage.cpp \
    main.cpp \

HEADERS += \
    Logic/MapImportAndExportManagers/cxmlmapimportandexportmanager.h \
    Logic/MapModels/ceditablemap.h \
    Logic/MapViews/cmapcreationview.h \
    Logic/MapViews/csimulationview.h \
    Logic/capplicationcontroller.h \
    Logic/cbasecontroller.h \
    Logic/MapModels/creadonlymap.h \
    Logic/MapViews/cbaseview.h \
    Logic/cmapcreationcontroller.h \
    Logic/csimulationcontroller.h \
    Logic/MapImportAndExportManagers/imapimportandexportmanager.h \
    MapElements/RoadUsers/ccar.h \
    MapElements/StationaryMapElements/RoadElements/ccrossing.h \
    MapElements/StationaryMapElements/RoadElements/croadway.h \
    MapElements/StationaryMapElements/RoadElements/croadwayelement.h \
    MapElements/StationaryMapElements/RoadElements/cturn.h \
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
    UI/cexpandablewidget.h \
    UI/chomepage.h \
    UI/cmainwindow.h \
    UI/cmapcreationpage.h \
    UI/cmapelementsplacementwidget.h \
    UI/cmapselectionpage.h \
    UI/croadusersparametersinputwidget.h \
    UI/croadusersplacementwidget.h \
    UI/csimulationconfigurationdialog.h \
    UI/csimulationpage.h \
    UI/ctrafficlightsdurationinputwidget.h \
    UI/iuipage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    graphics.qrc

DISTFILES += \

