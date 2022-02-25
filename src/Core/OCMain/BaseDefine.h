#ifndef _OC_BASE_H__
#define _OC_BASE_H__

#if defined(_MSC_VER)
#ifndef OCMain_EXPORTS
#define _MainExport  __declspec(dllimport)
#else
#define _MainExport  __declspec(dllexport)
#endif
#else
#define _MainExport
#endif

#include "osg.h"
#include "IteratorWrapper.h"
#include "OCUtility/StringConverter.h"
#include "OCUtility/FileUtil.h"
#include "OCUtility/tinyxml.h"
#include "ThreadDefine.h"

namespace osgViewer
{
	class View;
}

namespace OC
{
	class CCoreConfig;
	class CMetaRecord;
	class ConfigManager;
	class CSceneNode;
	class CFeatureNode;
	class IFeature;
	class IFeatureEffect;
	class ILayer;
	class IRecord;
	class JsonPackage;
	class ModelUtil;
	class FeatureRecord;
	class OCPluginTool;
	class LogManager;
	class SimpleConfig;
	class OCGeomFieldInfo;
	class OCJoinFieldInfo;
	class OCBinaryFieldInfo;
	class OCBinaryField;
	class TemplateRecord;
	
	typedef enum _OC_DATATYPE
	{
		OC_DATATYPE_INTEGER,
		OC_DATATYPE_FLOAT,
		OC_DATATYPE_TEXT,
		OC_DATATYPE_BLOB,
		OC_DATATYPE_NULL,
		OC_DATATYPE_TIME,
		OC_DATATYPE_GEOM
	}OC_DATATYPE;
}

#define AppCfg Singleton(OC::ConfigManager).ApplicationINI()
#define AppCfgLoadInt(classname,param,def) param = AppCfg.getIntValue(OC::String(#classname) + "::" + #param,def)
#define AppCfgLoadBool(classname,param,def) param = AppCfg.getBoolValue(OC::String(#classname) + "::" + #param,def)
#define AppCfgLoadFloat(classname,param,def) param = (FLOAT)AppCfg.getDoubleValue(OC::String(#classname) + "::" + #param,def)
#define AppCfgLoadWString(classname,param,def) param = A2U(AppCfg.getStringValue(OC::String(#classname) + "::" + #param,def)).c_str()
#define AppCfgLoadString(classname,param,def) param = AppCfg.getStringValue(OC::String(#classname) + "::" + #param,def)
#define AppCfgSaveInt(classname,param) AppCfg.setConfig(OC::String(#classname) + "::" + #param,(int)param)
#define AppCfgSaveBool(classname,param) AppCfg.setConfig(OC::String(#classname) + "::" + #param,(bool)param)
#define AppCfgSaveFloat(classname,param) AppCfg.setConfig(OC::String(#classname) + "::" + #param,(double)param)
#define AppCfgSaveWString(classname,param) AppCfg.setConfig(OC::String(#classname) + "::" + #param,U2A(param.GetString()))
#define AppCfgSaveString(classname,param) AppCfg.setConfig(OC::String(#classname) + "::" + #param,param)

#endif