INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCZip)

SET(TARGET_H
    crypt.h
    ioapi.h
    iostd.h
    PakoZip.h
    unzip.h
    zip.h
    ZipDefine.h
    BZip2.h
    bz2/bzlib.h
    bz2/bzlib_private.h
)

SET(TARGET_SRC
   src/ioapi.cpp
   src/iostd.cpp
   src/iowin32.c
   src/PakoZip.cpp
   src/zip.c
   src/unzip.c
   src/BZip2.cpp
   bz2/blocksort.c
   bz2/bzlib.c
   bz2/compress.c
   bz2/crctable.c
   bz2/decompress.c
   bz2/huffman.c
   bz2/randtable.c
)

SET(TARGET_EXTERNAL_LIBRARIES 
    zlib
)

SETUP_GROUP()
SETUP_LIBRARY(${LIB_NAME})
