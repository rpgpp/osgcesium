#include "StringUtil.h"
#include "aes.h"
#include "md5.h"

#if _WIN32
#include "objbase.h"
#include <stdio.h>
#else

#endif

namespace OC
{
	std::string convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength)
	{
#if defined(WIN32) && !defined(__CYGWIN__)
		if (sourceLength == 0)
		{
			return std::string();
		}

		int destLen = WideCharToMultiByte(CP_UTF8, 0, source, sourceLength, 0, 0, 0, 0);
		if (destLen <= 0)
		{
			return std::string();
		}

		std::string sDest(destLen, '\0');
		destLen = WideCharToMultiByte(CP_UTF8, 0, source, sourceLength, &sDest[0], destLen, 0, 0);

		if (destLen <= 0)
		{
			return std::string();
		}

		return sDest;
#else
		return std::string();
#endif
	}
	std::string convertUTF16toUTF8(const std::wstring& s) { return convertUTF16toUTF8(s.c_str(), s.length()); }

	std::string convertStringFromCurrentCodePageToUTF8(const char* source, unsigned sourceLength)
	{
#if defined(WIN32) && !defined(__CYGWIN__)
		if (sourceLength == 0)
		{
			return std::string();
		}

		int utf16Length = MultiByteToWideChar(CP_ACP, 0, source, sourceLength, 0, 0);
		if (utf16Length <= 0)
		{
			return std::string();
		}

		std::wstring sUTF16(utf16Length, L'\0');
		utf16Length = MultiByteToWideChar(CP_ACP, 0, source, sourceLength, &sUTF16[0], utf16Length);
		if (utf16Length <= 0)
		{
			return std::string();
		}

		return convertUTF16toUTF8(sUTF16);
#else
		return source;
#endif
	}

	std::string convertStringFromCurrentCodePageToUTF8(const std::string& s) { return convertStringFromCurrentCodePageToUTF8(s.c_str(), s.length()); }


	const String StringUtil::BLANK;

	String StringUtil::hashMD5(String content)
	{
		return md5(content);
	}

	String StringUtil::encodeURI(String uri)
	{
#if _WIN32
		int len = (int)uri.length();
		wchar_t* pbuf = new wchar_t[len+1];
		int n = MultiByteToWideChar(CP_ACP,0, uri.c_str(),len, pbuf,len+1);
		pbuf[n] =0;
		int b_size = n*3;
		char* pbuf2 = new char[b_size];
		n = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pbuf, n, pbuf2,b_size, 0, 0);
		String EncodedStr = "";
		for (int i=0;i<n;i++)
		{
			int iCh = pbuf2[i];
			if (iCh&0x80)//�����ַ�
			{
				iCh +=256;
				int v1=iCh/16;
				int v2=iCh % 16;
				EncodedStr +='%';
				if (v1>=0&&v1<=9)
					EncodedStr +=(char)('0'+v1);//16��������
				else
					EncodedStr +=(char)('A'+v1-10);
				if (v2>=0&&v2<=9)
					EncodedStr +=(char)('0'+v2);
				else
					EncodedStr +=(char)('A'+v2-10);
			}
			else if(//��ĸ��ת�����ֲ�ת
				(iCh>='a'&&iCh<='z')||
				(iCh>='A'&&iCh<='Z')||
				(iCh>='0'&&iCh<='9'))
			{
				EncodedStr +=(char)(iCh);
			}
			else
			{
				int v1=iCh/16;
				int v2=iCh % 16;
				EncodedStr +='%';
				if (v1>=0&&v1<=9)
					EncodedStr +=(char)('0'+v1);//16��������
				else
					EncodedStr +=(char)('A'+v1-10);//16���ƴ�д��ĸ
				if (v2>=0&&v2<=9)
					EncodedStr +=(char)('0'+v2);
				else
					EncodedStr +=(char)('A'+v2-10);
			}
		}
		CORE_SAFE_DELETE_ARRAY(pbuf);
		CORE_SAFE_DELETE_ARRAY(pbuf2);
		return EncodedStr;
#else
		return BLANK;
#endif
	}

	String StringUtil::decodeURI(String uri)
	{
#if _WIN32
		int uriLength = (int)uri.length();
		char *buffer = new char[uriLength];
		int pos = 0;
		for (int i=0;i < uriLength;i++)
		{
			int flag = 0;
			char ch = uri[i];
			if (ch == '%')
			{
				int i1=i+1;
				int i2=i+2;
				if (i2<uriLength&&uri[i+1]!='%')
				{
					int ch1 = (toupper(uri[i+1])-'0');//16���Ƶ�һλΪ����
					if (ch1>16)
						ch1 = (toupper(uri[i+1])-'A'+10);//16���Ƶ�һλΪ��ĸ
					ch1*=16;
					int ch2 =  toupper(uri[i+2])-'0';//16���Ƶڶ�λ
					if (ch2>16)
						ch2 =toupper(uri[i+2])-'A'+10;
					ch1+=ch2;
					if (ch1>256)
						ch1-=256;
					flag = ch1;
					i+=2;
				}
				else
					flag = ch;
			}
			else
				flag = ch;
			buffer[pos] = flag;
			pos++;
		}
		wchar_t* pbuf = new wchar_t[pos+1];
		int n = MultiByteToWideChar(CP_UTF8,0,buffer,pos,pbuf,pos+1);
		char* str2 = new char[n*3];
		n = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pbuf, n, str2,n*3, 0, 0);
		char *str3 = new char[n+1];
		for (int i=0;i<n;i++)
		{
			str3[i]=str2[i];
		}
		str3[n]='\0';
		String out(str3);
		CORE_SAFE_DELETE_ARRAY(buffer);
		CORE_SAFE_DELETE_ARRAY(pbuf);
		CORE_SAFE_DELETE_ARRAY(str2);
		CORE_SAFE_DELETE_ARRAY(str3);
		return out;
#else
		return BLANK;
#endif
	}

	String StringUtil::hexDecode(String hexStr)
	{
		StringStream result;
		char buf[2];
		for (size_t i = 0; i < hexStr.length(); i += 2)
		{
			buf[0] = hexStr[i];
			buf[1] = hexStr[i + 1];
			result << static_cast<unsigned char>(strtoul(buf, NULL, 16));
		}
		return result.str();
	}

	String StringUtil::hexEncode(String str)
	{
		static const String hex = "0123456789ABCDEF";
		StringStream result;
		for (size_t i = 0; i < str.length(); ++i)
			result << hex[(unsigned char)str[i] >> 4] << hex[(unsigned char)str[i] & 0xf];
		return result.str();
	}

	String StringUtil::aesDecode(String SECRET_KEY,String aeskey)
	{
		String result;
		result.resize(aeskey.length());
		AES aes;
		aes.setMode((unsigned char*)SECRET_KEY.c_str(), (int)SECRET_KEY.length(), aeskey.length(), true);
		aes.invCipher((const unsigned char*)aeskey.c_str(), (unsigned char*)result.c_str());
		return result;
	}

	String StringUtil::aesEncode(String SECRET_KEY, String str)
	{
		String result;
		result.resize(str.length());
		AES aes;
		aes.setMode((unsigned char*)SECRET_KEY.c_str(), (int)SECRET_KEY.length(), str.length(), true);
		aes.cipher((const unsigned char*)str.c_str(), (unsigned char*)result.c_str());
		return result;
	}

	String StringUtil::GenTime()
	{
		time_t t = time(0);
		char tmp[64];
		strftime( tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&t) );
		return tmp;
	}

	String StringUtil::GenGUID()
	{
#if _WIN32
		static char buf[64] = {0};  
		GUID guid;  
		if (S_OK == ::CoCreateGuid(&guid))  
		{  
			_snprintf(buf, sizeof(buf)  
				, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"  
				, guid.Data1  
				, guid.Data2  
				, guid.Data3  
				, guid.Data4[0], guid.Data4[1]  
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]  
			, guid.Data4[6], guid.Data4[7]  
			);  
		}
		return String((const char*)buf);  
#else
		static unsigned int _uuid = 0;
		StringStream s;
		s << _uuid++;
		return s.str();
#endif
	}


	void StringUtil::trim(String& str, bool left, bool right,String delims)
	{
		if(right)
			str.erase(str.find_last_not_of(delims)+1); // trim right
		if(left)
			str.erase(0, str.find_first_not_of(delims)); // trim left
	}

	StringVector StringUtil::split( const String& str, const String& delims, unsigned int maxSplits, bool preserveDelims)
	{
		StringVector ret;
		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

		unsigned int numSplits = 0;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do 
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back( str.substr(start) );
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back( str.substr(start, pos - start) );

				if(preserveDelims)
				{
					// Sometimes there could be more than one delimiter in a row.
					// Loop until we don't find any more delims
					size_t delimStart = pos, delimPos;
					delimPos = str.find_first_not_of(delims, delimStart);
					if (delimPos == String::npos)
					{
						// Copy the rest of the string
						ret.push_back( str.substr(delimStart) );
					}
					else
					{
						ret.push_back( str.substr(delimStart, delimPos - delimStart) );
					}
				}

				start = pos + 1;
			}
			// parse up to next real data
			start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != String::npos);



		return ret;
	}

	StringVector StringUtil::splitEx(const String& str, const String& delims, unsigned int maxSplits, bool preserveDelims)
	{
		StringVector ret;
		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

		unsigned int numSplits = 0;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
				if (pos != 0)
				{
					ret.push_back(BLANK);
				}
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back(str.substr(start));
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back(str.substr(start, pos - start));

				if (preserveDelims)
				{
					// Sometimes there could be more than one delimiter in a row.
					// Loop until we don't find any more delims
					size_t delimStart = pos, delimPos;
					delimPos = str.find_first_not_of(delims, delimStart);
					if (delimPos == String::npos)
					{
						// Copy the rest of the string
						ret.push_back(str.substr(delimStart));
					}
					else
					{
						ret.push_back(str.substr(delimStart, delimPos - delimStart));
					}
				}

				start = pos + 1;
			}
			// parse up to next real data
			//start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != String::npos);



		return ret;
	}

	//-----------------------------------------------------------------------
	StringVector StringUtil::tokenise( const String& str, const String& singleDelims, const String& doubleDelims, unsigned int maxSplits)
	{
		StringVector ret;
		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

		unsigned int numSplits = 0;
		String delims = singleDelims + doubleDelims;

		// Use STL methods 
		size_t start, pos;
		char curDoubleDelim = 0;
		start = 0;
		do 
		{
			if (curDoubleDelim != 0)
			{
				pos = str.find(curDoubleDelim, start);
			}
			else
			{
				pos = str.find_first_of(delims, start);
			}

			if (pos == start)
			{
				char curDelim = str.at(pos);
				if (doubleDelims.find_first_of(curDelim) != String::npos)
				{
					curDoubleDelim = curDelim;
				}
				// Do nothing
				start = pos + 1;
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				if (curDoubleDelim != 0)
				{
					//Missing closer. Warn or throw exception?
				}
				// Copy the rest of the string
				ret.push_back( str.substr(start) );
				break;
			}
			else
			{
				if (curDoubleDelim != 0)
				{
					curDoubleDelim = 0;
				}

				// Copy up to delimiter
				ret.push_back( str.substr(start, pos - start) );
				start = pos + 1;
			}
			if (curDoubleDelim == 0)
			{
				// parse up to next real data
				start = str.find_first_not_of(singleDelims, start);
			}

			++numSplits;

		} while (start != String::npos);

		return ret;
	}

	//-----------------------------------------------------------------------
	void StringUtil::toLowerCase(String& str)
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			tolower);
	}

	//-----------------------------------------------------------------------
	void StringUtil::toUpperCase(String& str) 
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			toupper);
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String startOfThis = str.substr(0, patternLen);
		if (lowerCase)
			StringUtil::toLowerCase(startOfThis);

		return (startOfThis == pattern);
	}
	//-----------------------------------------------------------------------
	bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String endOfThis = str.substr(thisLen - patternLen, patternLen);
		if (lowerCase)
			StringUtil::toLowerCase(endOfThis);

		return (endOfThis == pattern);
	}
	//-----------------------------------------------------------------------
	String StringUtil::standardisePath(const String& init)
	{
		String path = init;

		std::replace( path.begin(), path.end(), '\\', '/' );
		if( path[path.length() - 1] != '/' )
			path += '/';

		return path;
	}
	//-----------------------------------------------------------------------
	String StringUtil::normalizeFilePath(const String& init, bool makeLowerCase)
	{
		const char* bufferSrc = init.c_str();
		int pathLen = (int)init.size();
		int indexSrc = 0;
		int indexDst = 0;
		int metaPathArea = 0;

		char reservedBuf[1024];
		char* bufferDst = reservedBuf;
		bool isDestAllocated = false;
		if (pathLen > 1023)
		{
			//if source path is to long ensure we don't do a buffer overrun by allocating some
			//new memory
			isDestAllocated = true;
			bufferDst = new char[pathLen + 1];
		}

		//The outer loop loops over directories
		while (indexSrc < pathLen)
		{		
			if ((bufferSrc[indexSrc] == '\\') || (bufferSrc[indexSrc] == '/'))
			{
				//check if we have a directory delimiter if so skip it (we should already
				//have written such a delimiter by this point
				++indexSrc;
				continue;
			}
			else
			{
				//check if there is a directory to skip of type ".\"
				if ((bufferSrc[indexSrc] == '.') && 
					((bufferSrc[indexSrc + 1] == '\\') || (bufferSrc[indexSrc + 1] == '/')))
				{
					indexSrc += 2;
					continue;			
				}

				//check if there is a directory to skip of type "..\"
				else if ((bufferSrc[indexSrc] == '.') && (bufferSrc[indexSrc + 1] == '.') &&
					((bufferSrc[indexSrc + 2] == '\\') || (bufferSrc[indexSrc + 2] == '/')))
				{
					if (indexDst > metaPathArea)
					{
						//skip a directory backward in the destination path
						do {
							--indexDst;
						}
						while ((indexDst > metaPathArea) && (bufferDst[indexDst - 1] != '/'));
						indexSrc += 3;
						continue;
					}
					else
					{
						//we are about to write "..\" to the destination buffer
						//ensure we will not remove this in future "skip directories"
						metaPathArea += 3;
					}
				}
			}

			//transfer the current directory name from the source to the destination
			while (indexSrc < pathLen)
			{
				char curChar = bufferSrc[indexSrc];
				if (makeLowerCase) curChar = tolower(curChar);
				if ((curChar == '\\') || (curChar == '/')) curChar = '/';
				bufferDst[indexDst] = curChar;
				++indexDst;
				++indexSrc;
				if (curChar == '/') break;
			}
		}
		bufferDst[indexDst] = 0;

		String normalized(bufferDst); 
		if (isDestAllocated)
		{
			delete[] bufferDst;
		}

		return normalized;		
	}
	//-----------------------------------------------------------------------
	void StringUtil::splitFilename(const String& qualifiedName, 
		String& outBasename, String& outPath)
	{
		String path = qualifiedName;
		// Replace \ with / first
		std::replace( path.begin(), path.end(), '\\', '/' );
		// split based on final /
		size_t i = path.find_last_of('/');

		if (i == String::npos)
		{
			outPath.clear();
			outBasename = qualifiedName;
		}
		else
		{
			outBasename = path.substr(i+1, path.size() - i - 1);
			outPath = path.substr(0, i+1);
		}

	}
	//-----------------------------------------------------------------------
	void StringUtil::splitBaseFilename(const String& fullName, 
		String& outBasename, String& outExtention)
	{
		size_t i = fullName.find_last_of(".");
		if (i == String::npos)
		{
			outExtention.clear();
			outBasename = fullName;
		}
		else
		{
			outExtention = fullName.substr(i+1);
			outBasename = fullName.substr(0, i);
		}
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------
	void StringUtil::splitFullFilename(	const String& qualifiedName, 
		String& outBasename, String& outExtention, String& outPath )
	{
		String fullName;
		splitFilename( qualifiedName, fullName, outPath );
		splitBaseFilename( fullName, outBasename, outExtention );
	}


	bool StringUtil::contains(const String& str,const String& keyword, bool caseSensitive)
	{
		String tmpStr = str;
		String tmpKeyword = keyword;
		if (!caseSensitive)
		{
			StringUtil::toLowerCase(tmpStr);
			StringUtil::toLowerCase(tmpKeyword);
		}

		return tmpStr.find(tmpKeyword) != tmpStr.npos;
	}

	//-----------------------------------------------------------------------
	bool StringUtil::match(const String& str, const String& pattern, bool caseSensitive)
	{
		String tmpStr = str;
		String tmpPattern = pattern;
		if (!caseSensitive)
		{
			StringUtil::toLowerCase(tmpStr);
			StringUtil::toLowerCase(tmpPattern);
		}

		String::const_iterator strIt = tmpStr.begin();
		String::const_iterator patIt = tmpPattern.begin();
		String::const_iterator lastWildCardIt = tmpPattern.end();
		while (strIt != tmpStr.end() && patIt != tmpPattern.end())
		{
			if (*patIt == '*')
			{
				lastWildCardIt = patIt;
				// Skip over looking for next character
				++patIt;
				if (patIt == tmpPattern.end())
				{
					// Skip right to the end since * matches the entire rest of the string
					strIt = tmpStr.end();
				}
				else
				{
					// scan until we find next pattern character
					while(strIt != tmpStr.end() && *strIt != *patIt)
						++strIt;
				}
			}
			else
			{
				if (*patIt != *strIt)
				{
					if (lastWildCardIt != tmpPattern.end())
					{
						// The last wildcard can match this incorrect sequence
						// rewind pattern to wildcard and keep searching
						patIt = lastWildCardIt;
						lastWildCardIt = tmpPattern.end();
					}
					else
					{
						// no wildwards left
						return false;
					}
				}
				else
				{
					++patIt;
					++strIt;
				}
			}

		}
		// If we reached the end of both the pattern and the string, we succeeded
		if (patIt == tmpPattern.end() && strIt == tmpStr.end())
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	//-----------------------------------------------------------------------
	const String StringUtil::replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
	{
		String result = source;
		String::size_type pos = 0;
		while(1)
		{
			pos = result.find(replaceWhat,pos);
			if (pos == String::npos) break;
			result.replace(pos,replaceWhat.size(),replaceWithWhat);
			pos += replaceWithWhat.size();
		}
		return result;
	}

	String	StringUtil::ANSIToUTF8( const String str)
	{
		return convertStringFromCurrentCodePageToUTF8(str);
	}

	String	StringUtil::UnicodeToANSI( const WString str)
	{
#if _WIN32
		char*     pElementText;
		int    iTextLen;
		// wide char to multi char
		iTextLen = WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0,
			NULL,
			NULL );
		pElementText = new char[iTextLen + 1];
		memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
		::WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			pElementText,
			iTextLen,
			NULL,
			NULL );
		String strText;
		strText = pElementText;
		delete[] pElementText;
		return strText;
#else
		return StringUtil::BLANK;
#endif
		
	}
	WString	StringUtil::ANSIToUnicode( const String str)
	{
#if _WIN32
		int len = str.length();
		int unicodeLen = ::MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,NULL,0);  
		wchar_t* buffer = new wchar_t[unicodeLen + 1];  
		memset(buffer,0,(unicodeLen + 1)*sizeof(wchar_t));  
		::MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,(LPWSTR)buffer,unicodeLen);  
		WString rt(buffer);  
		delete[] buffer; 
		return rt; 
#else
		return WString(L"");
#endif
	}
}