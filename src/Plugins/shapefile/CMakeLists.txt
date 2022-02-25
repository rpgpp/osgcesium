SET(TARGET_SRC
    ShapeFileArchive.cpp
    ShapeFileDatabase.cpp
)

SET(TARGET_H
    ShapeFileArchive.h
    ShapeFileDatabase.h
)

SET(TARGET_ADDED_LIBRARIES OCModel osgEarth)

SET(TARGET_EXTERNAL_LIBRARIES 
    gdal_i
)

SETUP_PLUGIN(shapefile)
