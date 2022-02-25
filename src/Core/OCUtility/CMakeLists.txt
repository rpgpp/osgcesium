INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_NAME OCUtility)

SET(TARGET_H
   aes.h
   md5.h
   AxisAlignedBox.h
   ColourValue.h
   FileUtil.h
   MathUtil.h
   Matrix3.h
   Matrix4.h
   Plane.h
   PlaneBoundedVolume.h
   Polygon.h
   Quaternion.h
   Ray.h
   Rectangle.h
   Sphere.h
   StringConverter.h
   StringUtil.h
   tinystr.h
   tinyxml.h
   UtilityDefine.h
   Vector2.h
   Vector3.h
   Vector3i.h
   Vector4.h
   RGBAEncoder.h
)

SET(TARGET_SRC
   src/aes.cpp
   src/md5.cpp
   src/AxisAlignedBox.cpp
   src/ColourValue.cpp
   src/FileUtil.cpp
   src/MathUtil.cpp
   src/Matrix3.cpp
   src/Matrix4.cpp
   src/Plane.cpp
   src/Polygon.cpp
   src/Quaternion.cpp
   src/Rectangle.cpp
   src/StringConverter.cpp
   src/StringUtil.cpp
   src/tinystr.cpp
   src/tinyxml.cpp
   src/tinyxmlerror.cpp
   src/tinyxmlparser.cpp
   src/Vector2.cpp
   src/Vector3.cpp
   src/Vector4.cpp
   src/RGBAEncoder.cpp
)

ADD_DEPENDENCIES(OCMain ${LIB_NAME})

IF(WIN32)
ADD_DEPENDENCIES(OCServer ${LIB_NAME})
ENDIF()

SETUP_LIBRARY(${LIB_NAME})
