INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCHttpServer)

SET(TARGET_H
    WebSocket.h
    HttpServer.h
    HttpServerDefine.h
    MapCacher.h
)

SET(TARGET_SRC
    src/WebSocket.cc
    src/HttpServer.cpp
    src/mapcacheController.cpp
    src/MapCacher.cpp
    src/OCserverController.cpp
    src/MusicAPIController.cpp
)

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgEarth
    OCUtility
    OCMain
    OCServer
    OCHttpClient
    drogon
    trantor
    jsoncpp
)

SET(TARGET_EXTERNAL_LIBRARIES 
    ws2_32
    Rpcrt4
    zlib
    curl
)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++17")
    endif()
endif()

IF(WIN32)
ADD_DEPENDENCIES(Weathertoy ${LIB_NAME})
ADD_DEPENDENCIES(OCServerApp ${LIB_NAME})
ENDIF(WIN32)


SETUP_LIBRARY(${LIB_NAME})
