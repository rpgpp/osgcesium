SET(TARGET_SRC
    NCArchive.cpp
)

SET(TARGET_H
    NCArchive.h
)

SET(TARGET_ADDED_LIBRARIES OCNetCDF OCesium)

SET(TARGET_EXTERNAL_LIBRARIES 
    zlib
)

SETUP_PLUGIN(nc)
