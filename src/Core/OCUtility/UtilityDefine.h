#ifndef _OsgCesium_UTILITY_DEFINE_H__
#define _OsgCesium_UTILITY_DEFINE_H__

#if defined(_MSC_VER)
#ifndef OCUtility_EXPORTS
#define _UtilityExport  __declspec(dllimport)
#else
#define _UtilityExport  __declspec(dllexport)
#endif
#else
#define _UtilityExport
#endif

#include "OCMain/Platform.h"
#include "OCMain/stl.h"

#pragma warning (disable : 4244)

class TiXmlNode;
class TiXmlElement;

namespace OC
{
	class CUtilityTool;
	class CAxisAlignedBox;
	class CDegree;
	class CMatrix3;
	class CMatrix4;
	class CRectangle;
	class CPlane;
	class CPlaneBoundedVolume;
	class CQuaternion;
	class CRay;
	class CSphere;
	class CVector2;
	class CVector3;
	class CVector4;

	typedef std::vector<CVector2>	Vector2List;
	typedef std::vector<CVector3>	Vector3List;
	typedef std::vector<CVector4>	Vector4List;
	template<class T> class SharedPtr;

	class CUniqueClassManager;
	class SimpleConfig;

	typedef double Real;
	typedef unsigned char uchar;

	template<typename T>
	struct DeleteVector
	{
		void operator()(const T& bi)
		{
			delete bi;
		}
	};

	template<typename T>
	struct DeleteMapSecond
	{
		void operator()(const T& bi)
		{
			delete bi.second;
		}
	};

#define CORE_DELETE_VECTOR(name,t) for_each(name.begin(), name.end(), DeleteVector<t::value_type>());name.clear();
#define CORE_DELETE_SECOND(name,t) for_each(name.begin(), name.end(), DeleteMapSecond<t::value_type>());name.clear();

#define Singleton(name) name::getSingleton()
#define SingletonPtr(name) name::getSingletonPtr()

#define CORE_NEW new 
#define CORE_DELETE delete
#define CORE_SAFE_DELETE(p) { if(p){delete(p);  (p)=NULL;} }
#define CORE_SAFE_DELETE_ARRAY(p) { if(p){delete[] (p);  (p)=NULL;} }
#define CORE_SAFE_RELEASE(p) { if(p){ (p)->Release();  (p)=NULL;} }
#define CORE_FRIEND_CLASS(name) friend class name

#ifdef _WIN32
#include <process.h>
#define CORE_CURRENT_PID _getpid()
#else
#   include <unistd.h>
#   include <sys/syscall.h>
#define CORE_CURRENT_PID gettid()
#endif

#define SingletonNormal 1
#define SingletonProcess 2

#if _WIN32
#define SingletonConfig SingletonProcess
#else
#define SingletonConfig SingletonNormal
#endif

#if CORE_PLATFORM == CORE_PLATFORM_LINUX || CORE_PLATFORM == CORE_PLATFORM_APPLE || CORE_PLATFORM == CORE_PLATFORM_APPLE_IOS || \
	CORE_PLATFORM == CORE_PLATFORM_ANDROID || CORE_PLATFORM == CORE_PLATFORM_NACL
#define HWND OC::String
#endif

}

#define U2A(str) OC::StringUtil::UnicodeToANSI(str)
#define A2U(str) OC::StringUtil::ANSIToUnicode(str)
#define A2U8(str) OC::StringUtil::ANSIToUTF8(str)
#define CORE_MAKE_DIR(dir) if (OC::FileUtil::FileType(dir) == OC::FileUtil::FILE_NOT_FOUND){OC::FileUtil::makeDirectory(dir);}

#define Vector2ToVec2(v) osg::Vec2(v.x,v.y)
#define Vector3ToVec3(v) osg::Vec3(v.x,v.y,v.z)
#define Vector3ToVec3d(v) osg::Vec3d(v.x,v.y,v.z)
#define Vector4ToVec4(v) osg::Vec4(v.x,v.y,v.z,v.w)
#define ColorToVec4(v) osg::Vec4(v.r,v.g,v.b,v.a)
#define Vec2ToVector2(v) OC::CVector2(v[0],v[1])
#define Vec3ToVector3(v) OC::CVector3(v[0],v[1],v[2])
#define Vec4ToVector4(v) OC::CVector4(v[0],v[1],v[2],v[3])
#define Vec4ToColor(v) OC::ColourValue(v[0],v[1],v[2],v[3])
#define QuaternionToQuat(q) osg::Quat(q.x,q.y,q.z,q.w)
#define QuatToQuaterinon(q) OC::CQuaternion(q.w(),q.x(),q.y(),q.z())

#pragma warning(disable:4100)

template<class T> class SharedPtr;

class CUniqueClassManager;
class SimpleConfig;

typedef double Real;
typedef unsigned char uchar;

template<typename T>
struct DeleteVector
{
	void operator()(const T& bi)
	{
		delete bi;
	}
};

template<typename T>
struct DeleteMapSecond
{
	void operator()(const T& bi)
	{
		delete bi.second;
	}
};

#define CORE_DELETE_VECTOR(name,t) for_each(name.begin(), name.end(), DeleteVector<t::value_type>());name.clear();
#define CORE_DELETE_SECOND(name,t) for_each(name.begin(), name.end(), DeleteMapSecond<t::value_type>());name.clear();

#define Singleton(name) name::getSingleton()
#define SingletonPtr(name) name::getSingletonPtr()

#define CORE_NEW new 
#define CORE_DELETE delete
#define CORE_SAFE_DELETE(p) { if(p){delete(p);  (p)=NULL;} }
#define CORE_SAFE_DELETE_ARRAY(p) { if(p){delete[] (p);  (p)=NULL;} }
#define CORE_SAFE_RELEASE(p) { if(p){ (p)->Release();  (p)=NULL;} }
#define CORE_FRIEND_CLASS(name) friend class name

#ifdef _WIN32
#include <process.h>
#define CORE_CURRENT_PID _getpid()
#else
#   include <unistd.h>
#   include <sys/syscall.h>
#define CORE_CURRENT_PID gettid()
#endif

#define SingletonNormal 1
#define SingletonProcess 2

#if _WIN32
#define SingletonConfig SingletonProcess
#else
#define SingletonConfig SingletonNormal
#endif

#if CORE_PLATFORM == CORE_PLATFORM_LINUX || CORE_PLATFORM == CORE_PLATFORM_APPLE || CORE_PLATFORM == CORE_PLATFORM_APPLE_IOS || \
	CORE_PLATFORM == CORE_PLATFORM_ANDROID || CORE_PLATFORM == CORE_PLATFORM_NACL
#define HWND OC::String
#endif
#endif