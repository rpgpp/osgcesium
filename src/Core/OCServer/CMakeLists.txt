INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/OpenSceneGraph/include)
INCLUDE_DIRECTORIES(${WORKINGPATH}/OSGEarth/src)

SET(LIB_NAME OCServer)

SET(TARGET_H
    invokeCommand.h
    OCRegister.h
    OCServer.h
    OCServerDefine.h
    VolumeManager.h

)

SET(TARGET_SRC
    src/invokeCommand.cpp
    src/OCRegister.cpp
    src/OCServer.cpp
    src/VolumeManager.cpp
)

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgEarth
    OCUtility
    OCLayer
    OCMain
    OCVolume
)

ADD_DEPENDENCIES(OCHttpServer ${LIB_NAME})

SETUP_LIBRARY(${LIB_NAME})
