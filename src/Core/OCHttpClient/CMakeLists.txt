INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCHttpClient)

SET(TARGET_H
    HttpClientDefine.h
    HttpClient.h
    HTTPRequest.h
    HTTPResponse.h
)

SET(TARGET_SRC
    src/HttpClient.cpp
    src/HTTPRequest.cpp
    src/HTTPResponse.cpp
)

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgEarth
    OCUtility
    OCMain
    OCServer
)

SET(TARGET_EXTERNAL_LIBRARIES 
    curl
)

SETUP_LIBRARY(${LIB_NAME})
ADD_DEPENDENCIES(OCHttpServer ${LIB_NAME})
