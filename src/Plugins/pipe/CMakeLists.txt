SET(TARGET_SRC
    PipeArchive.cpp
    PipeTable.cpp
    LineTable.cpp
    NodeTable.cpp
    PipeLine.cpp
    PipeNode.cpp
    NodeExInfo.cpp
    PipeModelFactory.cpp
)

SET(TARGET_H
    PipeArchive.h
    PipeTable.h
    LineTable.h
    NodeTable.h
    PipeLine.h
    PipeNode.h
    NodeExInfo.h
    PipeModelFactory.h
)

SET(TARGET_ADDED_LIBRARIES
    osg
    osgDB
    OCModel
)

SET(TARGET_EXTERNAL_LIBRARIES 
    zlib
)

SETUP_PLUGIN(pipe)
