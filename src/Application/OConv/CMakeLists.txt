INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OConv)

SET(TARGET_H
    Geography/ShpDrawer.h
    Geography/ShpDrawerHelper.h
    Geography/Matrix.h
    Geography/Utils.h
    Geography/Variogram.h
    Geography/Vector3.h
    Geography/Kriging.h
    Geography/GDALTool.h
    Geography/GeographyTool.h
    Thread/ConvWorkqueue.h
    ConvCommand.h
    RadarStationKML.h
    SqliteOper.h
    OConvMapCacher.h
)

SET(TARGET_SRC
    Geography/ShpDrawer.cpp
    Geography/Kriging.cpp
    Geography/GDALTool.cpp
    Geography/GeographyTool.cpp
    Thread/ConvWorkqueue.cpp
    ConvCommand.cpp
    ConvCommand2.cpp
    tdtiles.cpp
    deprecated.cpp
    lic.cpp
    main.cpp
    RadarStationKML.cpp
    SqliteOper.cpp
    MapCacheClient.cpp
    OConvMapCacher.cpp
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
    OCLayer
    OCModel
    OCHttpServer
    OCHttpClient
)

SET(TARGET_EXTERNAL_LIBRARIES 
    gdal_i
    curl
)

SETUP_APPLICATION(${LIB_NAME})
SETUP_GROUP()