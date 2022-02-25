#ifndef __Platform_H_
#define __Platform_H_

namespace OC {
/* Initial platform/compiler-related stuff to set.
*/
#define CORE_PLATFORM_WIN32 1
#define CORE_PLATFORM_LINUX 2
#define CORE_PLATFORM_APPLE 3
#define CORE_PLATFORM_APPLE_IOS 4
#define CORE_PLATFORM_ANDROID 5
#define CORE_PLATFORM_NACL 6

#define CORE_COMPILER_MSVC 1
#define CORE_COMPILER_GNUC 2
#define CORE_COMPILER_BORL 3
#define CORE_COMPILER_WINSCW 4
#define CORE_COMPILER_GCCE 5
#define CORE_COMPILER_CLANG 6

#define CORE_ENDIAN_LITTLE 1
#define CORE_ENDIAN_BIG 2

#define CORE_ARCHITECTURE_32 1
#define CORE_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( __GCCE__ )
#   define CORE_COMPILER CORE_COMPILER_GCCE
#   define CORE_COMP_VER _MSC_VER
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE 
#elif defined( __WINSCW__ )
#   define CORE_COMPILER CORE_COMPILER_WINSCW
#   define CORE_COMP_VER _MSC_VER
#elif defined( _MSC_VER )
#   define CORE_COMPILER CORE_COMPILER_MSVC
#   define CORE_COMP_VER _MSC_VER
#elif defined( __clang__ )
#   define CORE_COMPILER CORE_COMPILER_CLANG
#   define CORE_COMP_VER (((__clang_major__)*100) + \
        (__clang_minor__*10) + \
        __clang_patchlevel__)
#elif defined( __GNUC__ )
#   define CORE_COMPILER CORE_COMPILER_GNUC
#   define CORE_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)
#elif defined( __BORLANDC__ )
#   define CORE_COMPILER CORE_COMPILER_BORL
#   define CORE_COMP_VER __BCPLUSPLUS__
#   define __FUNCTION__ __FUNC__ 
#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

#define CORE_COMPILER_MIN_VERSION(COMPILER, VERSION) (CORE_COMPILER == (COMPILER) && CORE_COMP_VER >= (VERSION))

	/* See if we can use __forceinline or if we need to use __inline instead */
#if CORE_COMPILER_MIN_VERSION(CORE_COMPILER_MSVC, 1200)
#define CORE_FORCE_INLINE __forceinline
#elif CORE_COMPILER_MIN_VERSION(CORE_COMPILER_GNUC, 340)
#define CORE_FORCE_INLINE inline __attribute__((always_inline))
#else
#define CORE_FORCE_INLINE __inline
#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if CORE_COMPILER == CORE_COMPILER_MSVC
#   if CORE_COMP_VER >= 1200
#       define FORCEINLINE __forceinline
#   endif
#elif defined(__MINGW32__)
#   if !defined(FORCEINLINE)
#       define FORCEINLINE __inline
#   endif
#else
#   define FORCEINLINE __inline
#endif

/* Finds the current platform */

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define CORE_PLATFORM CORE_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
    // Device                                                     Simulator
    // Both requiring OS version 4.0 or greater
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define CORE_PLATFORM CORE_PLATFORM_APPLE_IOS
#   else
#       define CORE_PLATFORM CORE_PLATFORM_APPLE
#   endif
#elif defined(__ANDROID__)
#	define CORE_PLATFORM CORE_PLATFORM_ANDROID
#elif defined( __native_client__ ) 
#   define CORE_PLATFORM CORE_PLATFORM_NACL
#   ifndef CORE_STATIC_LIB
#       error OGRE must be built as static for NaCl (CORE_STATIC=true in CMake)
#   endif
#   ifdef CORE_BUILD_RENDERSYSTEM_D3D9
#       error D3D9 is not supported on NaCl (CORE_BUILD_RENDERSYSTEM_D3D9 false in CMake)
#   endif
#   ifdef CORE_BUILD_RENDERSYSTEM_GL
#       error OpenGL is not supported on NaCl (CORE_BUILD_RENDERSYSTEM_GL=false in CMake)
#   endif
#   ifndef CORE_BUILD_RENDERSYSTEM_GLES2
#       error GLES2 render system is required for NaCl (CORE_BUILD_RENDERSYSTEM_GLES2=false in CMake)
#   endif
#else
#   define CORE_PLATFORM CORE_PLATFORM_LINUX
#endif

    /* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define CORE_ARCH_TYPE CORE_ARCHITECTURE_64
#else
#   define CORE_ARCH_TYPE CORE_ARCHITECTURE_32
#endif

// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define CORE_QUOTE_INPLACE(x) # x
#define CORE_QUOTE(x) CORE_QUOTE_INPLACE(x)
#define CORE_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

//----------------------------------------------------------------------------
// Windows Settings
#if CORE_PLATFORM == CORE_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( CORE_STATIC_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define _CommonExport
#   	define _CoreExport
#   	define _CorePrivate
#   else
#   	if defined( COMMON_NONCLIENT_BUILD )
#       	define _CommonExport __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define _CommonExport
#           else
#       	    define _CommonExport __declspec( dllimport )
#           endif
#   	endif
#   	if defined( CORE_NONCLIENT_BUILD )
#       	define _CoreExport __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define _CoreExport
#           else
#       	    define _CoreExport __declspec( dllimport )
#           endif
#   	endif
#   	define _CorePrivate
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define CORE_DEBUG_MODE 1
#   else
#       define CORE_DEBUG_MODE 0
#   endif

// Disable unicode support on MingW for GCC 3, poorly supported in stdlibc++
// STLPORT fixes this though so allow if found
// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLBOX_UNICODE__ in _mingw.h
// GCC 4 is also fine
#if defined(__MINGW32__)
# if CORE_COMP_VER < 400
#  if !defined(_STLPORT_VERSION)
#   include<_mingw.h>
#   if defined(__MINGW32_TOOLBOX_UNICODE__) || CORE_COMP_VER > 345
#    define CORE_UNICODE_SUPPORT 1
#   else
#    define CORE_UNICODE_SUPPORT 0
#   endif
#  else
#   define CORE_UNICODE_SUPPORT 1
#  endif
# else
#  define CORE_UNICODE_SUPPORT 1
# endif
#else
#  define CORE_UNICODE_SUPPORT 1
#endif

#endif // CORE_PLATFORM == CORE_PLATFORM_WIN32

//----------------------------------------------------------------------------
// Android Settings
/*
#if CORE_PLATFORM == CORE_PLATFORM_ANDROID
#   define _CoreExport 
#	define CORE_UNICODE_SUPPORT 1
#   define CORE_DEBUG_MODE 0
#   define _CorePrivate
#	  define CLOCKS_PER_SEC  1000
//  pragma def were found here: http://www.inf.pucrs.br/~eduardob/disciplinas/SistEmbarcados/Mobile/Nokia/Tools/Carbide_vs/WINSCW/Help/PDF/C_Compilers_Reference_3.2.pdf
#	  pragma warn_unusedarg off
#	  pragma warn_emptydecl off
#	  pragma warn_possunwant off
// A quick define to overcome different names for the same function
#   define stricmp strcasecmp
#   ifdef DEBUG
#       define CORE_DEBUG_MODE 1
#   else
#       define CORE_DEBUG_MODE 0
#   endif
#endif
*/
//----------------------------------------------------------------------------
// Linux/Apple/iOs/Android/NaCl Settings
#if CORE_PLATFORM == CORE_PLATFORM_LINUX || CORE_PLATFORM == CORE_PLATFORM_APPLE || CORE_PLATFORM == CORE_PLATFORM_APPLE_IOS || \
    CORE_PLATFORM == CORE_PLATFORM_ANDROID || CORE_PLATFORM == CORE_PLATFORM_NACL

// Enable GCC symbol visibility
#   if defined( CORE_GCC_VISIBILITY )
#       define _CommonExport  __attribute__ ((visibility("default")))
#       define _CoreExport  __attribute__ ((visibility("default")))
#       define _CorePrivate __attribute__ ((visibility("hidden")))
#   else
#       define _CommonExport
#       define _CoreExport
#       define _CorePrivate
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

#   ifdef DEBUG
#       define CORE_DEBUG_MODE 1
#   else
#       define CORE_DEBUG_MODE 0
#   endif

#if CORE_PLATFORM == CORE_PLATFORM_APPLE
    #define CORE_PLATFORM_LIB "OgrePlatform.bundle"
#elif CORE_PLATFORM == CORE_PLATFORM_APPLE_IOS
    #define CORE_PLATFORM_LIB "OgrePlatform.a"
#else //CORE_PLATFORM_LINUX
    #define CORE_PLATFORM_LIB "libOgrePlatform.so"
#endif

// Always enable unicode support for the moment
// Perhaps disable in old versions of gcc if necessary
#define CORE_UNICODE_SUPPORT 1

#endif

//----------------------------------------------------------------------------
// Endian Settings
// check for BIG_ENDIAN config flag, set CORE_ENDIAN correctly
#ifdef CORE_CONFIG_BIG_ENDIAN
#    define CORE_ENDIAN CORE_ENDIAN_BIG
#else
#    define CORE_ENDIAN CORE_ENDIAN_LITTLE
#endif

// Integer formats of fixed bit width
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;
// define uint64 type
#if CORE_COMPILER == CORE_COMPILER_MSVC
	typedef unsigned __int64 uint64;
	typedef __int64 int64;
#else
	typedef unsigned long long uint64;
	typedef long long int64;
#endif

}

#endif
