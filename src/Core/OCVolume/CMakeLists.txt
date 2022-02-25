INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCVolume)

SET(TARGET_H
    VolumeDefine.h
    VolumeObject.h
    CinRadHeader.h
    CinRadReader.h
    CinRadReaderFMT.h
    Volume.h
    Swan.h
    Vtk.h
    CinRadC.h
    CinRad.h
    FyAwx.h
)

SET(TARGET_SRC
   src/VolumeObject.cpp
   src/CinRadReader.cpp
   src/CinRadReaderFMT.cpp
   src/Swan.cpp
   src/Volume.cpp
   src/Vtk.cpp
   src/CinRadC.cpp
   src/CinRad.cpp
   src/FyAwx.cpp
)

SET(TARGET_LIBRARIES
    OpenThreads
    osg
    osgDB
    OCZip
    OCMain
    OCUtility
)

SET(TARGET_EXTERNAL_LIBRARIES 
    zlib
)

SETUP_LIBRARY(${LIB_NAME})
