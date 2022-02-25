#ifndef _CORE_CONFIG_CLASS_H__
#define _CORE_CONFIG_CLASS_H__

#include "BaseDefine.h"
#include "OCUtility/Rectangle.h"
#include "OCUtility/Vector2.h"
#include "OCUtility/Vector3.h"
#include "OCUtility/Vector4.h"
#include "OCUtility/ColourValue.h"
#include "OCUtility/StringUtil.h"

namespace OC
{
	class _MainExport CCoreConfig
	{
	public:
		CCoreConfig(void);
		virtual	~CCoreConfig(void);

		CCoreConfig(const CCoreConfig& refConfig);
		inline CCoreConfig& operator = ( const CCoreConfig& rkSect);

		void save();
		bool fromFile(String filename);
		bool keyExists(String name);

		int getIntValue(String name,int defaultVal = 0);
		bool getBoolValue(String name,bool defaultVal = false);
		double getDoubleValue(String name,double defaultVal = 0.0);
		String getStringValue(String name,String defaultVal = StringUtil::BLANK);
		CVector2 getVector2Value(String name,CVector2 defalutVal = CVector2::ZERO);
		CVector3 getVector3Value(String name,CVector3 defalutVal = CVector3::ZERO);
		CVector4 getVector4Value(String name,CVector4 defalutVal = CVector4::ZERO);
		CRectangle getRectangle(String name,CRectangle defalutVal = CRectangle::ZERO);
		ColourValue getColourValue(String name,ColourValue defalutVal = ColourValue::White);

		void setConfig(String name,int val);
		void setConfig(String name,bool val);
		void setConfig(String name,double val);
		void setConfig(String name,String val);
		void setConfig(String name,CVector2 val);
		void setConfig(String name,CVector3 val);
		void setConfig(String name,CVector4 val);
		void setConfig(String name,CRectangle val);
		void setConfig(String name,ColourValue val);

		SimpleConfig* get();
	protected:
		SimpleConfig*	mConfig;
	};
}



#endif

