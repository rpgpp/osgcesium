#ifndef _STRING_UTIL_CLASS_H_
#define _STRING_UTIL_CLASS_H_

#include "UtilityDefine.h"

namespace OC
{
	class _UtilityExport StringUtil
	{
	public:
		typedef StringStream StrStreamType;
		static String hashMD5(String content);
		static String encodeURI(String uri);
		static String decodeURI(String uri);

		static String GenGUID();
		static String GenTime();
		static String hexDecode(String hexStr);
		static String hexEncode(String str);

		static String aesDecode(String SECRET_KEY, String hexStr);
		static String aesEncode(String SECRET_KEY, String str);

		static void trim( String& str, bool left = true, bool right = true, String delims = " \t\r");
		
		static std::vector<String> split(const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);
		static std::vector<String> splitEx(const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);

		static std::vector<String> tokenise( const String& str, const String& delims = "\t\n ", const String& doubleDelims = "\"", unsigned int maxSplits = 0);
		
        static void toLowerCase( String& str );
        
		static void toUpperCase( String& str );

		static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Returns whether the string ends with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the end of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Method for standardising paths - use forward slashes only, end with slash.
        */
        static String standardisePath( const String &init);
		/** Returns a normalized version of a file path
		This method can be used to make file path strings which point to the same directory  
		but have different texts to be normalized to the same text. The function:
		- Transforms all backward slashes to forward slashes.
		- Removes repeating slashes.
		- Removes initial slashes from the beginning of the path.
		- Removes ".\" and "..\" meta directories.
		- Sets all characters to lowercase (if requested)
		@param init The file path to normalize.
		@param makeLowerCase If true, transforms all characters in the string to lowercase.
		*/
       static String normalizeFilePath(const String& init, bool makeLowerCase = true);


        /** Method for splitting a fully qualified filename into the base name
            and path.
        @remarks
            Path is standardised as in standardisePath
        */
        static void splitFilename(const String& qualifiedName,
            String& outBasename, String& outPath);

		/** Method for splitting a fully qualified filename into the base name,
		extension and path.
		@remarks
		Path is standardised as in standardisePath
		*/
		static void splitFullFilename(const String& qualifiedName, 
			String& outBasename, String& outExtention, 
			String& outPath);

		/** Method for splitting a filename into the base name
		and extension.
		*/
		static void splitBaseFilename(const String& fullName, 
			String& outBasename, String& outExtention);


		static bool contains(const String& str,const String& keyword, bool caseSensitive = true);

		static bool match(const String& str, const String& pattern, bool caseSensitive = true);

		static const String replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

		static String	UnicodeToANSI( const WString str);
		static String	ANSIToUTF8( const String str);
		
		static WString	ANSIToUnicode( const String str);
		
		static const String BLANK;
	};
}

#endif
