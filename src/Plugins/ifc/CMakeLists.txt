SET(TARGET_SRC
    IFCArchive.cpp
    IFCConverter.cpp
)

SET(TARGET_H
    IFCArchive.h
    IFCConverter.h
)

SET(TARGET_ADDED_LIBRARIES IfcPlusPlus carve osgText)

INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc)
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc/boost)
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc/ifcplusplus)
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/etc/ifcplusplus/carve/include)

SETUP_PLUGIN(ifc)
