SET(TARGET_SRC
    ConvertToVRML.cpp
    ReaderWriterVRML2.cpp
    Primitives.cpp
    IndexedFaceSet.cpp
)

SET(TARGET_H
    ConvertToVRML.h
    ReaderWriterVRML2.h
)

SET(TARGET_EXTERNAL_LIBRARIES 
    openvrml
)

ADD_DEFINITIONS(-DOPENVRML_USE_DLL)

INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc/boost)
LINK_DIRECTORIES(${WORKINGPATH}/3rdParty/etc/boost/lib)

SETUP_PLUGIN(vrl)
