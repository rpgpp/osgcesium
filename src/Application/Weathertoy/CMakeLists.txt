INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc)
SET(LIB_NAME Weathertoy)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QRC_SOURCE_FILES resources.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})
SOURCE_GROUP("Resource Files" FILES ${QRC_SOURCE_FILES})

SET(TARGET_H
    Bridge.h
    LabViewer.h
    LabWebEngineView.h
    RibbonMenuWidget.h
    ServerThread.h
    WeatherLabMenu.h
    webuihandler.h
    RadarStationKML.h
)

SET(TARGET_SRC
    Bridge.cpp
    LabViewer.cpp
    LabWebEngineView.cpp
    RibbonMenuWidget.cpp
    ServerThread.cpp
    WeatherLabMenu.cpp
    webuihandler.cpp
    main.cpp
    WeatherLab.rc
    ${QRC_FILES}
)
SET(TARGET_COMMON_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgUtil
    osgEarth
    OCZip
    OCVolume
    OCUtility
    OCMain
    OCesium
    OCModel
    OCHttpServer
    SARibbonBar
)
SETUP_APPLICATION(${LIB_NAME})

if (MSVC)
  set_property(TARGET ${LIB_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${OUTPUT_BINDIR})
endif()

qt5_use_modules( ${LIB_NAME} Widgets )
qt5_use_modules( ${LIB_NAME} Xml )
qt5_use_modules( ${LIB_NAME} WebEngine )
qt5_use_modules( ${LIB_NAME} WebView )
qt5_use_modules( ${LIB_NAME} WebEngineWidgets )
