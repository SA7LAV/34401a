cmake_minimum_required(VERSION 3.22)
project(hp34401a VERSION 1.1.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

qt_add_executable(hp34401a
    src/main.cpp
    src/models.cpp
    src/config.cpp
    src/translations.cpp
    src/instrument.cpp
    src/ui/displaywidget.cpp
    src/ui/modepanel.cpp
    src/ui/rangepanel.cpp
    src/ui/connectiondialog.cpp
    src/ui/overlaywindow.cpp
    src/ui/setupdialog.cpp
    src/ui/mainwindow.cpp
    assets.qrc
)

target_include_directories(hp34401a PRIVATE src)

target_compile_definitions(hp34401a PRIVATE APP_VERSION="${PROJECT_VERSION}")

target_link_libraries(hp34401a PRIVATE Qt6::Widgets Qt6::SerialPort)

install(TARGETS hp34401a DESTINATION bin)
install(FILES hp34401a.desktop DESTINATION share/applications)
install(FILES assets/hp34401a.png DESTINATION share/pixmaps)
