#include "CoreConfig.h"
#include "SimpleConfig.h"
#include "OCUtility/StringConverter.h"
#include "OCUtility/tinyxml.h"

namespace OC
{
	CCoreConfig::CCoreConfig(void)
	{
		mConfig = new SimpleConfig;
	}

	CCoreConfig::~CCoreConfig(void)
	{
		CORE_SAFE_DELETE(mConfig);
	}

	CCoreConfig::CCoreConfig(const CCoreConfig& refConfig)
	{
		mConfig = new SimpleConfig;
		*mConfig = *refConfig.mConfig;
	}

	CCoreConfig& CCoreConfig::operator = ( const CCoreConfig& rkSect)
	{
		*mConfig = *rkSect.mConfig;
		return *this;
	}

	SimpleConfig* CCoreConfig::get()
	{
		return mConfig;
	}

	void CCoreConfig::save()
	{
		mConfig->SaveFile();
	}

	bool CCoreConfig::fromFile(String filename)
	{
		try
		{
			mConfig->ReadFile(filename);
			return true;
		}
		catch (...){}

		return false;
	}

	bool CCoreConfig::keyExists(String name)
	{
		return mConfig->KeyExists(name);
	}

	int CCoreConfig::getIntValue(String name,int defaultVal)
	{
		return StringConverter::parseInt(getStringValue(name),defaultVal);
	}

	bool CCoreConfig::getBoolValue(String name,bool defaultVal)
	{
		return StringConverter::parseBool(getStringValue(name),defaultVal);
	}

	double CCoreConfig::getDoubleValue(String name,double defaultVal)
	{
		return StringConverter::parseReal(getStringValue(name),defaultVal);
	}
	String CCoreConfig::getStringValue(String name,String defaultVal)
	{
		return mConfig->Read(name,defaultVal);
	}

	CVector2 CCoreConfig::getVector2Value(String name,CVector2 defalutVal)
	{
		return StringConverter::parseVector2(getStringValue(name),defalutVal);
	}

	CVector3 CCoreConfig::getVector3Value(String name,CVector3 defalutVal)
	{
		return StringConverter::parseVector3(getStringValue(name),defalutVal);
	}

	CVector4 CCoreConfig::getVector4Value(String name,CVector4 defalutVal)
	{
		return StringConverter::parseVector4(getStringValue(name),defalutVal);
	}

	CRectangle CCoreConfig::getRectangle(String name,CRectangle defalutVal)
	{
		return StringConverter::parseRectangle(getStringValue(name),defalutVal);
	}

	ColourValue CCoreConfig::getColourValue(String name,ColourValue defalutVal)
	{
		return StringConverter::parseColourValue(getStringValue(name),defalutVal);
	}

	void CCoreConfig::setConfig(String name,int val)
	{
		mConfig->Add(name,val);
	}

	void CCoreConfig::setConfig(String name,bool val)
	{
		setConfig(name,StringConverter::toString(val));
	}

	void CCoreConfig::setConfig(String name,double val)
	{
		mConfig->Add(name,val);
	}

	void CCoreConfig::setConfig(String name,String val)
	{
		mConfig->Add(name,val);
	}

	void CCoreConfig::setConfig(String name,CVector2 val)
	{
		setConfig(name,StringConverter::toString(val));
	}

	void CCoreConfig::setConfig(String name,CVector3 val)
	{
		setConfig(name,StringConverter::toString(val));
	}

	void CCoreConfig::setConfig(String name,CVector4 val)
	{
		setConfig(name,StringConverter::toString(val));
	}

	void CCoreConfig::setConfig(String name,CRectangle val)
	{
		setConfig(name,StringConverter::toString(val));
	}

	void CCoreConfig::setConfig(String name,ColourValue val)
	{
		setConfig(name,StringConverter::toString(val));
	}
}
