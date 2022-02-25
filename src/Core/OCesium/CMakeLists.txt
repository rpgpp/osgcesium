INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${WORKINGPATH}/3rdParty/include)

SET(LIB_NAME OCesium)

SET(TARGET_H
    allocators.h
    B3dmWriter.h
    BaseWriter.h
    BatchGeode.h
    BatchGroup.h
    CesiumDefine.h
    CesiumTool.h
    CmptWriter.h
    cursorstreamwrapper.h
    document.h
    DracoEncoder.h
    encodedstream.h
    encodings.h
    error/en.h
    error/error.h
    FeatureAttributeCallback.h
    FeatureBatchTable.h
    filereadstream.h
    filewritestream.h
    fwd.h
    gltfConfig.h
    GltfHelper.h
    gltfMaterial.h
    GLTFObject.h
    I3dmWriter.h
    internal/biginteger.h
    internal/clzll.h
    internal/diyfp.h
    internal/dtoa.h
    internal/ieee754.h
    internal/itoa.h
    internal/meta.h
    internal/pow10.h
    internal/regex.h
    internal/stack.h
    internal/strfunc.h
    internal/strtod.h
    internal/swap.h
    istreamwrapper.h
    KtxCompress.h
    KtxOptions.h
    memorybuffer.h
    memorystream.h
    MergeTriangleOperator.h
    msinttypes/inttypes.h
    msinttypes/stdint.h
    ostreamwrapper.h
    PntsWriter.h
    pointer.h
    prettywriter.h
    rapidjson.h
    reader.h
    schema.h
    stb_image.h
    stb_image_write.h
    stream.h
    stringbuffer.h
    TextureAtlasBuilder.h
    TextureOptimize.h
    TinyGltfDelegate.h
    tiny_gltf.h
    WebpEncoder.h
    writer.h
    X3DM.h
    TDTiles.h
    MapProject.h
    BuildingConverter.h
    ConvertWorkqueue.h
    gltfAttributeCallback.h
    glTFConverter.h
    glTFOptimize.h
    ObliqueConverter.h
)

SET(TARGET_SRC
   src/B3dmWriter.cpp
   src/BaseWriter.cpp
   src/BatchGeode.cpp
   src/BatchGroup.cpp
   src/CmptWriter.cpp
   src/DracoEncoder.cpp
   src/FeatureAttributeCallback.cpp
   src/FeatureBatchTable.cpp
   src/gltfConfig.cpp
   src/gltfMaterial.cpp
   src/KtxCompress.cpp
   src/CesiumTool.cpp
   src/GltfHelper.cpp
   src/GLTFObject.cpp
   src/I3dmWriter.cpp
   src/PntsWriter.cpp
   src/TinyGltfDelegate.cpp
   src/X3DM.cpp
   src/TextureAtlasBuilder.cpp
   src/TextureOptimize.cpp
   src/WebpEncoder.cpp
   src/image.cc
   src/TDTiles.cpp
   src/MapProject.cpp
   converter/BuildingConverter.cpp
   converter/ConvertWorkqueue.cpp
   converter/gltfAttributeCallback.cpp
   converter/glTFConverter.cpp
   converter/glTFOptimize.cpp
   converter/ObliqueConverter.cpp
)


SETUP_GROUP()

SET(TARGET_LIBRARIES 
    OpenThreads
    osg
    osgDB
    osgEarth
    OCUtility
    OCMain
    OCLayer
    OCZip
    ktx
    draco
    objUtil
)

ADD_DEFINITIONS(-DTINYGLTF_ENABLE_DRACO -DKHRONOS_STATIC -DTINYGLTF_USE_RAPIDJSON_CRTALLOCATOR)

SET(TARGET_EXTERNAL_LIBRARIES 
    webp
)

IF(WIN32)
    ADD_DEPENDENCIES(OConv ${LIB_NAME})
ENDIF()

IF(Qt5Widgets_FOUND)
    ADD_DEPENDENCIES(Weathertoy ${LIB_NAME})
ENDIF()

SETUP_LIBRARY(${LIB_NAME})
