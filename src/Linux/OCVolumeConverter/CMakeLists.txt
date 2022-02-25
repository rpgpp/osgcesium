INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_NAME OCVolumeConverter)

SET(TARGET_H
    fetcher.h
    watcher.h
    WatcherManager.h
    TaskWorkQueue.h
    Tool.h
)

SET(TARGET_SRC
    fetcher.cpp   
    watcher.cpp   
    WatcherManager.cpp
    TaskWorkQueue.cpp
    Tool.cpp
    main.cpp
)

SET(TARGET_COMMON_LIBRARIES 
    OpenThreads
    osg
    osgDB
    OCVolume
    OCUtility
    OCMain
)

SETUP_APPLICATION(${LIB_NAME})
