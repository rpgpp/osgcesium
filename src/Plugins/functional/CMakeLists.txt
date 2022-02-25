SET(TARGET_SRC
    FuncArchive.cpp
)

SET(TARGET_H
    FuncArchive.h
)

SET(TARGET_ADDED_LIBRARIES
    osg
    osgDB
    osgEarth
)

SET(TARGET_EXTERNAL_LIBRARIES 
    zlib
)

SETUP_PLUGIN(func)
