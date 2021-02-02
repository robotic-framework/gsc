cmake_minimum_required(VERSION 3.17)
project(GCS)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Serialport)
find_package(Qt5Svg)
find_package(Qt5Webenginewidgets)
find_package(Qt5Webchannel)
find_package(Qt5Bluetooth)

set(UI_FILES mainwindow.ui optiondialog.ui)

set(RC_FILES)

set(QRC_FILES resources.qrc)

set(SOURCE_FILES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        optiondialog.h
        optiondialog.cpp
        connector.h
        connector_bridge.h
        connector_bridge.cpp
        ble.h
        ble.cpp
        serial.h
        serial.cpp
        serializer.h
        serializer.cpp
        protocol.h
        protocol.cpp
        webview_bridge.h
        webview_bridge.cpp
)

add_executable(${PROJECT_NAME} ${RC_FILES} ${SOURCE_FILES} ${UI_FILES} ${QRC_FILES})

include_directories(${PROJECT_SOURCE_DIR}/../PlaneDashboard)

link_directories(${PROJECT_SOURCE_DIR}/../build-PlaneDashboard-Desktop_Qt_5_15_2_clang_64bit-Release)

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::Gui
        Qt5::SerialPort
        Qt5::Svg
        Qt5::WebEngineWidgets
        Qt5::WebChannel
        Qt5::Bluetooth
        ${PROJECT_SOURCE_DIR}/../build-PlaneDashboard-Desktop_Qt_5_15_2_clang_64bit-Release/libplanedashboardplugin.dylib
)
