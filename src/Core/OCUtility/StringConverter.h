#ifndef _STRING_CONVERTER_CLASS_H_
#define _STRING_CONVERTER_CLASS_H_

#include "StringUtil.h"
#include "ColourValue.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Rectangle.h"

namespace OC
{
	class _UtilityExport StringConverter
	{
	public:
		static String formatDoubletoString(double val,int precision = 2);
		static String toString(double val, unsigned short precision = 6, 
			unsigned short width = 0, char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );

		/** Converts an int to a String. */
		static String toString(int val, unsigned short width = 0, 
			char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );
#if _WIN32
		/** Converts a size_t to a String. */
		static String toString(size_t val, 
			unsigned short width = 0, char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );
#endif

		/** Converts an unsigned long to a String. */
		static String toString(unsigned long val, 
			unsigned short width = 0, char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );

		/** Converts a long to a String. */
		static String toString(long val, 
			unsigned short width = 0, char fill = ' ', 
			std::ios::fmtflags flags = std::ios::fmtflags(0) );

		static String toString(const CVector2& val);
		static String toString(const CVector3& val);
		static String toString(const CVector4& val);
		static String toString(const CRectangle& val);
		static String toString(const std::vector<double>& val);

		/** Converts a boolean to a String. 
        @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
        */
		static String toString(bool val, bool yesNo = false);

		/** Converts a StringVector to a string.
        @remarks
            Strings must not contain spaces since space is used as a delimiter in
            the output.
        */

        static String toString(const StringVector& val);

		static String toString(const CMatrix4& val);
		static String toString(const CQuaternion& val);
		static String toString(const ColourValue& val);

		 /** Converts a String to a Real. 
        @return
            0.0 if the value could not be parsed, otherwise the Real version of the String.
        */
        static double parseReal(const String& val, double defaultValue = 0);
		/** Converts a String to a whole number. 
        @return
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static int parseInt(const String& val, int defaultValue = 0);
        /** Converts a String to a whole number. 
        @return
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue = 0);
        /** Converts a String to a whole number. 
        @return
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static long parseLong(const String& val, long defaultValue = 0);
        /** Converts a String to a whole number. 
        @return
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue = 0);
        /** Converts a String to size_t. 
        @return
            defaultValue if the value could not be parsed, otherwise the numeric version of the String.
        */
        static size_t parseSizeT(const String& val, size_t defaultValue = 0);
        /** Converts a String to a boolean. 
        @remarks
            Returns true if case-insensitive match of the start of the string
			matches "true", "yes" or "1", false otherwise.
        */
        static bool parseBool(const String& val, bool defaultValue = 0);
		/** Parses a Vector2 out of a String.
        @remarks
            Format is "x y" ie. 2 Real components, space delimited. Failure to parse returns
            Vector2::ZERO.
        */

		/** Pareses a StringVector from a string.
        @remarks
            Strings must not contain spaces since space is used as a delimiter in
            the output.
        */
        static StringVector parseStringVector(const String& val);

        /** Checks the String is a valid number value. */
        static bool isNumber(const String& val);

		static ColourValue parseColourValue(const String& val, const ColourValue& defaultValue = ColourValue::Black);
		
		static CVector2 parseVector2(const String& val, const CVector2& defaultValue = CVector2::ZERO);
		static CVector3 parseVector3(const String& val, const CVector3& defaultValue = CVector3::ZERO);
		static CVector4 parseVector4(const String& val, const CVector4& defaultValue = CVector4::ZERO);
		static CRectangle parseRectangle(const String& val, const CRectangle& defaultValue = CRectangle::ZERO);
		static CQuaternion parseQuaternion(const String& val, const CQuaternion& defaultValue = CQuaternion::IDENTITY);
		static std::vector<double> parseDoubleVector(const String& val,const double &defaultValue = 0);
	};
}

#endif


