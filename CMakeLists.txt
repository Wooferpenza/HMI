cmake_minimum_required(VERSION 3.16)

project(HMI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialBus)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        variable.h
        variable.cpp
        registerutils.h
        modbuscommon.h
        modbusmanager.h
        modbusmanager.cpp
        modbusmodel.h
        modbusmodel.cpp
        modbussettings.h
        modbussettings.cpp
        modbussettingsdialog.h
        modbussettingsdialog.cpp
        modbussettingsdialog.ui
        numericdisplay.h
        numericdisplay.cpp
        momentarybutton.h
        momentarybutton.cpp
        togglebutton.h
        togglebutton.cpp
        multistatebutton.h
        multistatebutton.cpp
        numpaddialog.h
        numpaddialog.cpp
        numpaddialog.ui
)

qt_add_executable(HMI
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    variableproperty.h
)

target_include_directories(HMI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(HMI PRIVATE
    Qt6::Widgets
    Qt6::SerialBus
)

set_target_properties(HMI PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HMI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(HMI)
