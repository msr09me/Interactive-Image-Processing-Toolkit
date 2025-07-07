cmake_minimum_required(VERSION 3.16)

project(InteractiveImageProcessingToolkit_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(InteractiveImageProcessingToolkit_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        imageqtadapter.h imageqtadapter.cpp
    )
else()
    if(ANDROID)
        add_library(InteractiveImageProcessingToolkit_GUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(InteractiveImageProcessingToolkit_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Include algorithm headers
target_include_directories(InteractiveImageProcessingToolkit_GUI
    PRIVATE
    ${CMAKE_SOURCE_DIR}/AlgorithmImplementation/include
)

# Link Qt and your static core library
target_link_libraries(InteractiveImageProcessingToolkit_GUI
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    core
)

# Optional for bundle setups
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.InteractiveImageProcessingToolkit_GUI)
endif()

set_target_properties(InteractiveImageProcessingToolkit_GUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS InteractiveImageProcessingToolkit_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(InteractiveImageProcessingToolkit_GUI)
endif()
