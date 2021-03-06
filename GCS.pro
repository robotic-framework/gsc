QT       += core gui
QT       += serialport
QT       += svg
QT       += webenginewidgets webchannel
QT       += bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    ../PlaneDashboard/verticalprogress.cpp \
    ble.cpp \
    connector_bridge.cpp \
    main.cpp \
    main_window.cpp \
    option_dialog.cpp \
    protocol.cpp \
    serial.cpp \
    serializable_connector.cpp \
    serializer.cpp \
    webview_bridge.cpp

HEADERS += \
#    ../PlaneDashboard/verticalprogress.h \
    ble.h \
    connector.h \
    connector_bridge.h \
    main_window.h \
    option_dialog.h \
    protocol.h \
    serial.h \
    serializable_connector.h \
    serializer.h \
    webview_bridge.h

FORMS += \
    main_window.ui \
    option_dialog.ui

TRANSLATIONS += \
    GCS_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-PlaneDashboard-Desktop_Qt_5_15_2_clang_64bit-Release/release/ -lplanedashboardplugin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-PlaneDashboard-Desktop_Qt_5_15_2_clang_64bit-Release/debug/ -lplanedashboardplugin
else:unix: LIBS += -L$$PWD/../build-PlaneDashboard-Desktop_Qt_5_15_2_clang_64bit-Release/ -lplanedashboardplugin

INCLUDEPATH += $$PWD/../PlaneDashboard
DEPENDPATH += $$PWD/../PlaneDashboard

RESOURCES += \
    resources.qrc
