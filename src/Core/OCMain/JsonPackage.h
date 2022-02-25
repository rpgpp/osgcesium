#ifndef _OC_JSONPACKAGE_H__
#define _OC_JSONPACKAGE_H__

#include "BaseDefine.h"
#include "json/json.h"

namespace OC
{
	class _MainExport JsonPackage
	{
	public:
		JsonPackage();
		~JsonPackage();

		static int getInt(Json::Value& obj,String key,int defaultVal = 1);
		static bool getBool(Json::Value& obj,String key,bool defaultVal = false);
		static double getDouble(Json::Value& obj,String key,double defaultVal = 0.0);
		static uint32 getUInt(Json::Value& obj,String key,uint32 defaultVal = 1);
		static String getString(Json::Value& obj,String key,String defaultVal = StringUtil::BLANK);

		void addNameVal(String name,int val);
		void addNameVal(String name,long val);
		void addNameVal(String name,double val);
		void addNameVal(String name,String val);
		void addNameVal(String name, CVector2 val);
		void addNameVal(String name, CVector3 val);
		void addNameVal(String name,JsonPackage& package);

		String toJSON();
	private:
		Json::Value							mRoot;
		std::map<String,int>				mArrayCount;
	};
}


#endif
