INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCNetCDF)

SET(TARGET_H
    npy/cnpy.h
    npy/npyTemprature.h
    NcDBZ.h
    NcZQ.h
    NetCDFDefine.h
    NetCDFReader.h
    NcZT.h
    NcUV.h
    NcVolume.h
    NcepWind.h
)

SET(TARGET_SRC
    npy/cnpy.cpp
    npy/npyTemprature.cpp
    src/NcDBZ.cpp
    src/NcZQ.cpp
    src/NetCDFReader.cpp
    src/NcZT.cpp
    src/NcUV.cpp
    src/NcVolume.cpp
    src/NcepWind.cpp
)

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgEarth
    OCUtility
    OCMain
    OCVolume
)

SET(TARGET_EXTERNAL_LIBRARIES 
    gdal_i
    netcdf
    zlib
)

SETUP_GROUP()
SETUP_LIBRARY(${LIB_NAME})
