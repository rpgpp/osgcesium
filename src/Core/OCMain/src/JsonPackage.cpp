#include "JsonPackage.h"

namespace OC
{
	JsonPackage::JsonPackage()
	{
	}

	JsonPackage::~JsonPackage()
	{

	}

	bool JsonPackage::getBool(Json::Value& obj,String key,bool defaultVal)
	{
		Json::Value val = obj[key.c_str()];

		if (val.isNull())
		{
			return defaultVal;
		}

		try
		{
			return val.asBool();
		}
		catch (...){}

		return defaultVal;
	}

	uint32 JsonPackage::getUInt(Json::Value& obj,String key,uint32 defaultVal)
	{
		Json::Value val = obj[key.c_str()];

		if (val.isNull())
		{
			return defaultVal;
		}

		try
		{
			return val.asUInt();
		}
		catch (...){}

		return defaultVal;
	}

	int JsonPackage::getInt(Json::Value& obj,String key,int defaultVal)
	{
		Json::Value val = obj[key.c_str()];

		if (val.isNull())
		{
			return defaultVal;
		}

		try
		{
			return val.asInt();
		}
		catch (...){}

		return defaultVal;
	}

	double JsonPackage::getDouble(Json::Value& obj,String key,double defaultVal)
	{
		Json::Value val = obj[key.c_str()];

		if (val.isNull())
		{
			return defaultVal;
		}

		try
		{
			return val.asDouble();
		}
		catch (...){}

		return defaultVal;
	}

	String JsonPackage::getString(Json::Value& obj,String key,String defaultVal)
	{
		Json::Value val = obj[key.c_str()];

		if (val.isNull())
		{
			return defaultVal;
		}

		try
		{
			if (val.isNumeric())
			{
				return StringConverter::toString(val.asInt());
			}
			return val.asCString();
		}
		catch (...){}

		return defaultVal;
	}

	void JsonPackage::addNameVal(String name,int val)
	{
		mRoot[name.c_str()] = val;
	}

	void JsonPackage::addNameVal(String name,long val)
	{
		mRoot[name.c_str()] = (Json::Value::UInt)val;
	}

	void JsonPackage::addNameVal(String name,double val)
	{
		mRoot[name.c_str()] = val;
	}

	void JsonPackage::addNameVal(String name,String val)
	{
		mRoot[name.c_str()] = val;
	}

	void JsonPackage::addNameVal(String name, CVector2 val)
	{
		mRoot[name.c_str()]["x"] = val.x;
		mRoot[name.c_str()]["y"] = val.y;
	}

	void JsonPackage::addNameVal(String name,CVector3 val)
	{
		mRoot[name.c_str()]["x"] = val.x;
		mRoot[name.c_str()]["y"] = val.y;
		mRoot[name.c_str()]["z"] = val.z;
	}

	void JsonPackage::addNameVal(String name,JsonPackage& package)
	{
		Json::Value::Members mebs = package.mRoot.getMemberNames();
		Json::Value::Members::iterator it = mebs.begin();
		for (;it!=mebs.end();it++)
		{
			String key = *it;

			std::map<String,int>::iterator it = mArrayCount.find(key);
			if (it == mArrayCount.end())
			{
				mArrayCount[key] = 0;
			}

			mRoot[mArrayCount[key]++][name.c_str()][key.c_str()] = package.mRoot[key.c_str()];
		}
	}

	String JsonPackage::toJSON()
	{
		Json::FastWriter fw;
		try
		{
			return fw.write(mRoot);
		}
		catch (...){}

		return StringUtil::BLANK;
	}
}
