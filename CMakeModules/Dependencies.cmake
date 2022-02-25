set(OGRE_DEPENDENCIES_DIR "${PROJECT_BINARY_DIR}/Dependencies" CACHE PATH "Path to prebuilt OGRE dependencies")
option(OC_BUILD_DEPENDENCIES "automatically build Ogre Dependencies (SDL2, pugixml)" TRUE)

message(STATUS "DEPENDENCIES_DIR: ${OGRE_DEPENDENCIES_DIR}")

set(OGREDEPS_PATH "${OGRE_DEPENDENCIES_DIR}")
set(OGRE_DEP_SEARCH_PATH "${OGRE_DEPENDENCIES_DIR}")

if(CMAKE_CROSSCOMPILING)
    set(CMAKE_FIND_ROOT_PATH ${OGREDEPS_PATH} "${CMAKE_FIND_ROOT_PATH}")

    set(CROSS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
    
    if(ANDROID)
        set(CROSS ${CROSS}
            -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
            -DANDROID_ABI=${ANDROID_ABI}
            -DANDROID_NDK=${ANDROID_NDK})
    endif()
    
    if(APPLE_IOS)
        set(CROSS ${CROSS}
            -DIOS_PLATFORM=${IOS_PLATFORM})
    else()
        # this should help discovering zlib, but on ios it breaks it
        set(CROSS ${CROSS}
            -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH})
    endif()
endif()

# if we build our own deps, do it static as it generally eases distribution
set(OGREDEPS_SHARED FALSE)

set(BUILD_COMMAND_OPTS --target install --config ${CMAKE_BUILD_TYPE})

set(BUILD_COMMAND_COMMON ${CMAKE_COMMAND}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=${OGREDEPS_PATH}
  -G ${CMAKE_GENERATOR}
  -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
  -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
  -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE # allow linking into a shared lib
  ${CROSS})

# Set hardcoded path guesses for various platforms
if (UNIX AND NOT EMSCRIPTEN)
  set(OGRE_DEP_SEARCH_PATH ${OGRE_DEP_SEARCH_PATH} /usr/local)
  # Ubuntu 11.10 has an inconvenient path to OpenGL libraries
  set(OGRE_DEP_SEARCH_PATH ${OGRE_DEP_SEARCH_PATH} /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu)
endif ()

# give guesses as hints to the find_package calls
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${OGRE_DEP_SEARCH_PATH} E:/H3D/build/zlib-1.2.11)
set(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} ${OGRE_DEP_SEARCH_PATH})

#if(OC_BUILD_DEPENDENCIES AND NOT EXISTS ${OGREDEPS_PATH})
if(OC_BUILD_DEPENDENCIES)
    if(MSVC OR MINGW OR SKBUILD) # other platforms dont need this
#     message(STATUS "Building zlib")
#     file(DOWNLOAD
#         http://zlib.net/zlib-1.2.11.tar.gz
#         ${PROJECT_BINARY_DIR}/zlib-1.2.11.tar.gz
#         EXPECTED_MD5 1c9f62f0778697a09d36121ead88e08e)
#     execute_process(COMMAND ${CMAKE_COMMAND}
#         -E tar xf zlib-1.2.11.tar.gz WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
#     execute_process(COMMAND ${BUILD_COMMAND_COMMON}
#         -DBUILD_SHARED_LIBS=${OGREDEPS_SHARED}
#         ${PROJECT_BINARY_DIR}/zlib-1.2.11
#         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/zlib-1.2.11)
#     execute_process(COMMAND ${CMAKE_COMMAND}
#         --build ${PROJECT_BINARY_DIR}/zlib-1.2.11 ${BUILD_COMMAND_OPTS})
#
#    message(STATUS "Building curl")
#    execute_process(COMMAND ${CMAKE_COMMAND}
#        -E tar xf curl-7.80.0.tar.gz WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
#    execute_process(COMMAND ${BUILD_COMMAND_COMMON}
#    -DBUILD_SHARED_LIBS=${OGREDEPS_SHARED}
#    ${PROJECT_BINARY_DIR}/curl-7.80.0
#    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/curl-7.80.0)
#    execute_process(COMMAND ${CMAKE_COMMAND}
#     --build ${PROJECT_BINARY_DIR}/curl-7.80.0 ${BUILD_COMMAND_OPTS})
    endif()
endif()

# Find zlib
find_package(ZLIB)
macro_log_feature(ZLIB_FOUND "zlib" "Simple data compression library" "http://www.zlib.net" FALSE "" "")

#find_package(CURL)
#macro_log_feature(CURL_FOUND "curl" "" "https://curl.se" FALSE "" "")

#find_package(sqlite3)
#macro_log_feature(SQLITE3_FOUND "sqlite3" "" "" FALSE "" "")