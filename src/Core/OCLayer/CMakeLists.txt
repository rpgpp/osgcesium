INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_NAME OCLayer)

SET(TARGET_H
    Feature.h
    FeatureEffect.h
    FeatureLayer.h
    FeatureNode.h
    FeatureRecord.h
    ILayer.h
    LayerConfig.h
    LayerDefine.h
    LayerManager.h
    MetaRecord.h
    QuadTree.h
    OCResource.h
    ReadFileCallback.h
    IArchive.h
    IDatabase.h
    IDatabaseObject.h
    QueryInfo.h
    ModelUtil.h
    SceneNode.h
    ScenePass.h
    IRecord.h
    IFeature.h
)

SET(TARGET_SRC
    src/Feature.cpp
    src/FeatureEffect.cpp
    src/FeatureLayer.cpp
    src/FeatureNode.cpp
    src/FeatureRecord.cpp
    src/ILayer.cpp
    src/LayerConfig.cpp
    src/LayerManager.cpp
    src/MetaRecord.cpp
    src/QuadTree.cpp
    src/OCResource.cpp
    src/ReadFileCallback.cpp
    src/IArchive.cpp
    src/IDatabase.cpp
    src/IDatabaseObject.cpp
    src/QueryInfo.cpp
    src/ModelUtil.cpp
    src/SceneNode.cpp
    src/ScenePass.cpp
    src/IRecord.cpp
    src/IFeature.cpp
)

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgUtil
    OCUtility
    OCMain
)

IF(WIN32)
ADD_DEPENDENCIES(OCModel ${LIB_NAME})
ENDIF(WIN32)

SETUP_LIBRARY(${LIB_NAME})
