#include "StringConverter.h"

using namespace std;

namespace OC
{
	String StringConverter::formatDoubletoString(double val,int precision)	
	{
		if (Math::RealEqual(val,0))
		{
			val = 0.0f;
		}
		char buffer[128];
		String fm = "%4." + toString(precision) + "f";
#if WIN32
		sprintf_s(buffer,128,fm.c_str(),val);
		return String(buffer);
#else
		return StringConverter::toString(val);
#endif
	}

	String StringConverter::toString(const CMatrix4& val)
	{
		stringstream stream;
		stream << val[0][0] << " " 
			<< val[0][1] << " "             
			<< val[0][2] << " "             
			<< val[0][3] << " "             
			<< val[1][0] << " "             
			<< val[1][1] << " "             
			<< val[1][2] << " "             
			<< val[1][3] << " "             
			<< val[2][0] << " "             
			<< val[2][1] << " "             
			<< val[2][2] << " "             
			<< val[2][3] << " "             
			<< val[3][0] << " "             
			<< val[3][1] << " "             
			<< val[3][2] << " "             
			<< val[3][3];
		return stream.str();
	}

	String StringConverter::toString(const CQuaternion& val)
	{
		stringstream stream;
		stream  << val.w << " " << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String StringConverter::toString(const std::vector<double>& val)
	{
		StringStream stream;
		stream.precision(6);
		for(std::vector<double>::const_iterator itr = val.begin(); itr != val.end(); ++itr)
		{
			if(itr != val.end() - 1)
				stream<<*itr<<" ";
			else
				stream<<*itr;			
		}
		return stream.str();
	}

	String StringConverter::toString(const CVector2& val)
	{
		StringStream stream;
		stream.precision(6);
		stream << val.x << " " << val.y;
		return stream.str();
	}

	String StringConverter::toString(const CVector3& val)
	{
		StringStream stream;
		stream.precision(12);
		stream << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String StringConverter::toString(const CVector4& val)
	{
		stringstream stream;
		stream.precision(12);
		stream << val.x << " " << val.y << " " << val.z << " " << val.w;
		return stream.str();
	}

	String StringConverter::toString(const CRectangle& val)
	{
		stringstream stream;
		stream.precision(12);
		stream << val.getMinimum().x << " " << val.getMinimum().y << " " << val.getMaximum().x << " " << val.getMaximum().y;
		return stream.str();
	}

	String StringConverter::toString(double val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.precision(precision);
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringConverter::toString(int val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringConverter::toString(bool val, bool yesNo)
	{
		if (val)
		{
			if (yesNo)
			{
				return "yes";
			}
			else
			{
				return "true";
			}
		}
		else
			if (yesNo)
			{
				return "no";
			}
			else
			{
				return "false";
			}
	}
#if _WIN32
	String StringConverter::toString(size_t val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#endif
	//-----------------------------------------------------------------------
	String StringConverter::toString(unsigned long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringConverter::toString(long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringConverter::toString(const StringVector& val)
	{
		StringStream stream;
		StringVector::const_iterator i, iend, ibegin;
		ibegin = val.begin();
		iend = val.end();
		for (i = ibegin; i != iend; ++i)
		{
			if (i != ibegin)
				stream << " ";

			stream << *i; 
		}
		return stream.str();
	}
	//-----------------------------------------------------------------------
	double StringConverter::parseReal(const String& val, double defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		double ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;
		return ret;
	}
	int StringConverter::parseInt(const String& val, int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned int StringConverter::parseUnsignedInt(const String& val, unsigned int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned int ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	long StringConverter::parseLong(const String& val, long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		long ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned long StringConverter::parseUnsignedLong(const String& val, unsigned long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned long ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	size_t StringConverter::parseSizeT(const String& val, size_t defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		size_t ret = defaultValue;
		if( !(str >> ret) )
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	bool StringConverter::parseBool(const String& val, bool defaultValue)
	{
		if ((StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
			|| StringUtil::startsWith(val, "1")
			|| StringUtil::startsWith(val, "enable")
			|| StringUtil::startsWith(val,"��")))
			return true;
		else if ((StringUtil::startsWith(val, "false") || StringUtil::startsWith(val, "no")
			|| StringUtil::startsWith(val, "0")
			|| StringUtil::startsWith(val, "disable")
			||StringUtil::startsWith(val,"��")))
			return false;
		else
			return defaultValue;
	}
	//-----------------------------------------------------------------------
	StringVector StringConverter::parseStringVector(const String& val)
	{
		return StringUtil::split(val);
	}
	//-----------------------------------------------------------------------
	bool StringConverter::isNumber(const String& val)
	{
		StringStream str(val);
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
	}
	
	std::vector<double> StringConverter::parseDoubleVector(const String& val,const double &defaultValue)
	{
		std::vector<double> valueList;
		std::vector<String> vec = StringUtil::split(val);
		for(std::vector<String>::iterator itr = vec.begin(); itr != vec.end(); ++itr)
		{
			double value = StringConverter::parseReal(*itr,defaultValue);
			valueList.push_back(value);
		}
		return valueList;
	}

	//-----------------------------------------------------------------------
	ColourValue StringConverter::parseColourValue(const String& val, const ColourValue& defaultValue)
	{
		//1
		if (StringUtil::startsWith(val,"RGB",false))
		{
			String value = val.substr(4,val.length() - 5);

			StringVector rgbVector = StringUtil::split(value,",");

			if (rgbVector.size() == 3)
			{
				int r = StringConverter::parseInt(rgbVector[0]);
				int g = StringConverter::parseInt(rgbVector[1]);
				int b = StringConverter::parseInt(rgbVector[2]);
				return ColourValue(r/255.0,g/255.0,b/255.0, 1.0f);
			}
		}

		//2
		{
			String value = val;
			StringVector rgbVector = StringUtil::split(value,",");
			if (rgbVector.size() == 3)
			{
				int r = StringConverter::parseInt(rgbVector[0]);
				int g = StringConverter::parseInt(rgbVector[1]);
				int b = StringConverter::parseInt(rgbVector[2]);
				return ColourValue(r/255.0,g/255.0,b/255.0, 1.0f);
			}
			else if (rgbVector.size() == 4)
			{
				int r = StringConverter::parseInt(rgbVector[0]);
				int g = StringConverter::parseInt(rgbVector[1]);
				int b = StringConverter::parseInt(rgbVector[2]);
				int a = StringConverter::parseInt(rgbVector[3]);
				return ColourValue(r/255.0,g/255.0,b/255.0, a/255.0);
			}
		}


		//3
		// Split on space
		std::vector<String> vec = StringUtil::split(val);

		if (vec.size() == 4)
		{
			return ColourValue(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), parseReal(vec[3]));
		}
		else if (vec.size() == 3)
		{
			return ColourValue(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 1.0f);
		}
		else
		{
			return defaultValue;
		}
	}

	
	CVector2 StringConverter::parseVector2(const String& val, const CVector2& defaultValue)
	{
		// Split on space
		std::vector<String> vec = StringUtil::split(val);

		if (vec.size() != 2)
		{
			return defaultValue;
		}
		else
		{
			return CVector2(parseReal(vec[0]),parseReal(vec[1]));
		}
	}


	CVector3 StringConverter::parseVector3(const String& val, const CVector3& defaultValue)
	{
		// Split on space
		std::vector<String> vec = StringUtil::split(val);

		if (vec.size() != 3)
		{
			return defaultValue;
		}
		else
		{
			return CVector3(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]));
		}
	}

	CVector4 StringConverter::parseVector4(const String& val, const CVector4& defaultValue)
	{
		// Split on space
		vector<String> vec = StringUtil::split(val);

		if (vec.size() != 4)
		{
			return defaultValue;
		}
		else
		{
			return CVector4(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]),parseReal(vec[3]));
		}
	}

	CRectangle StringConverter::parseRectangle(const String& val, const CRectangle& defaultValue)
	{
		vector<String> vec = StringUtil::split(val);

		if (vec.size() != 4)
		{
			return defaultValue;
		}
		else
		{
			CRectangle rect(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]),parseReal(vec[3]));
			return rect;
		}
	}

	CQuaternion StringConverter::parseQuaternion(const String& val, const CQuaternion& defaultValue)
	{
		// Split on space
		std::vector<String> vec = StringUtil::split(val);

		if (vec.size() != 4)
		{
			return defaultValue;
		}
		else
		{
			return CQuaternion(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), parseReal(vec[3]));
		}
	}

	String StringConverter::toString(const ColourValue& val)
	{
		stringstream stream;
		stream << val.r << " " << val.g << " " << val.b << " " << val.a;
		return stream.str();
	}
}

