INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_NAME OCMain)

SET(TARGET_H
   json/assertions.h
   json/autolink.h
   json/config.h
   json/features.h
   json/forwards.h
   json/json.h
   json/reader.h
   json/value.h
   json/version.h
   json/writer.h
   json/json_tool.h
   BaseDefine.h
   BaseEnum.h
   ConfigManager.h
   CoreConfig.h
   Environment.h
   JsonPackage.h
   Singleton.h
   SimpleConfig.h
   Any.h
   AtomicScalar.h
   Common.h
   DataStream.h
   DefaultWorkQueueStandard.h
   Exception.h
   Log.h
   LogManager.h
   ThreadDefine.h
   Timer.h
   JsonHelper.h
   WorkQueue.h
   FunctionalArchive.h
   OCPluginTool.h
)

SET(TARGET_SRC
   json/json_reader.cpp
   json/json_value.cpp
   json/json_writer.cpp
   src/ConfigManager.cpp
   src/CoreConfig.cpp
   src/Environment.cpp
   src/JsonPackage.cpp
   src/AtomicScalar.cpp
   src/DataStream.cpp
   src/DefaultWorkQueueStandard.cpp
   src/Exception.cpp
   src/Log.cpp
   src/LogManager.cpp
   src/Timer.cpp
   src/JsonHelper.cpp
   src/WorkQueue.cpp
   src/FunctionalArchive.cpp
   src/OCPluginTool.cpp
)

SETUP_GROUP()

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    OCUtility
)

ADD_DEFINITIONS(-DJSON_DLL_BUILD)

ADD_DEPENDENCIES(OCVolume ${LIB_NAME})
IF(WIN32)
ADD_DEPENDENCIES(OCNetCDF ${LIB_NAME})
ENDIF(WIN32)
SETUP_LIBRARY(${LIB_NAME})
