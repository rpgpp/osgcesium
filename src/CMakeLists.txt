INCLUDE_DIRECTORIES(${WORKINGPATH}/OpenSceneGraph/build/include)
INCLUDE_DIRECTORIES(${WORKINGPATH}/OpenSceneGraph/include)
INCLUDE_DIRECTORIES(${WORKINGPATH}/osgearth/build/build_include)
INCLUDE_DIRECTORIES(${WORKINGPATH}/osgearth/src)
INCLUDE_DIRECTORIES(${WORKINGPATH}/src/Core)

IF (Qt5Widgets_FOUND)
    ADD_SUBDIRECTORY(Application/Weathertoy)
ENDIF()

IF(WIN32)
FOREACH(mylibfolder
    Application/OCServerApp
    Application/OConv
    Core/OCesium
    Core/OCModel
    Core/OCNetCDF
    Core/OCHttpServer
    Core/OCHttpClient
    Core/OCServer
    )
    ADD_SUBDIRECTORY(${mylibfolder})
ENDFOREACH()
ENDIF(WIN32)

FOREACH(mylibfolder
       Core/OCLayer
       Core/OCZip
       Core/OCVolume
       Core/OCMain
       Core/OCUtility
       Linux/OCVolumeConverter
    )
    ADD_SUBDIRECTORY(${WORKINGPATH}/src/${mylibfolder})
ENDFOREACH()

INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)
IF(WIN32)
ADD_SUBDIRECTORY(Plugins)
ENDIF(WIN32)
