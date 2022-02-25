#include "FileUtil.h"
#include <stack>


#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#define WINBASE_DECLARE_GET_MODULE_HANDLE_EX
#include <windows.h>
#include <winbase.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h> // for _mkdir

#define mkdir(x,y) _mkdir((x))

#if !defined(__MINGW32__)
#define stat64 _stati64
#endif

// set up for windows so acts just like unix access().
#ifndef F_OK
#define F_OK 4
#endif

#else // unix

#if defined( __APPLE__ )
// I'm not sure how we would handle this in raw Darwin
// without the AvailablilityMacros.
#include <AvailabilityMacros.h>

//>OSG_IOS
//IOS includes
#include "TargetConditionals.h"
#include <sys/cdefs.h>

#if (TARGET_OS_IPHONE)
#include <Availability.h>
// workaround a bug which appears when compiling for SDK < 4.0 and for the simulator
#if defined(__IPHONE_4_0) && (__IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_4_0)
#define stat64 stat
#else
#if !TARGET_IPHONE_SIMULATOR
#define stat64 stat
#endif
#endif
#endif
//<OSG_IPHONE

// 10.5 defines stat64 so we can't use this #define
// By default, MAC_OS_X_VERSION_MAX_ALLOWED is set to the latest
// system the headers know about. So I will use this as the control
// variable. (MIN_ALLOWED is set low by default so it is
// unhelpful in this case.)
// Unfortunately, we can't use the label MAC_OS_X_VERSION_10_4
// for older OS's like Jaguar, Panther since they are not defined,
// so I am going to hardcode the number.
#if (MAC_OS_X_VERSION_MAX_ALLOWED <= 1040)
#define stat64 stat
#endif
#elif defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__DragonFly__) || \
      (defined(__hpux) && !defined(_LARGEFILE64_SOURCE))
#define stat64 stat
#endif

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_DARWIN_FEATURE_64_BIT_INODE)
#define stat64 stat
#endif
#endif

#if defined(__ANDROID__)
#define stat64 stat
#endif

// set up _S_ISDIR()
#if !defined(S_ISDIR)
#  if defined( _S_IFDIR) && !defined( __S_IFDIR)
#    define __S_IFDIR _S_IFDIR
#  endif
#  define S_ISDIR(mode)    (mode&__S_IFDIR)
#endif

namespace OC
{
	static const char* const PATH_SEPARATORS = "/\\";
	static unsigned int PATH_SEPARATORS_LEN = 2;

	bool fileExists(const std::string& filename)
	{
		return access(filename.c_str(), F_OK) == 0;
	}

	std::string getFilePath(const std::string& fileName)
	{
		std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
		if (slash == std::string::npos) return std::string();
		else return std::string(fileName, 0, slash);
	}

#define OSGDB_STRING_TO_FILENAME(s) s
#define OSGDB_FILENAME_TO_STRING(s) s
#define OSGDB_FILENAME_TEXT(x) x
#define OSGDB_WINDOWS_FUNCT(x) x ## A
#define OSGDB_WINDOWS_FUNCT_STRING(x) #x "A"
#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#include <direct.h>

	StringVector getDirectoryContents(const std::string& dirName)
	{
		StringVector contents;

		OSGDB_WINDOWS_FUNCT(WIN32_FIND_DATA) data;
		HANDLE handle = OSGDB_WINDOWS_FUNCT(FindFirstFile)((OSGDB_STRING_TO_FILENAME(dirName) + OSGDB_FILENAME_TEXT("\\*")).c_str(), &data);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				contents.push_back(OSGDB_FILENAME_TO_STRING(data.cFileName));
			} while (OSGDB_WINDOWS_FUNCT(FindNextFile)(handle, &data) != 0);

			FindClose(handle);
		}
		return contents;
	}

#else

#include <dirent.h>
	StringVector getDirectoryContents(const std::string& dirName)
	{
		StringVector contents;

		DIR* handle = opendir(dirName.c_str());
		if (handle)
		{
			dirent* rc;
			while ((rc = readdir(handle)) != NULL)
			{
				contents.push_back(rc->d_name);
			}
			closedir(handle);
		}

		return contents;
	}

#endif // unix getDirectoryContexts

	FileUtil::Type fileType(const std::string& filename)
	{
		struct stat64 fileStat;
		if (stat64(filename.c_str(), &fileStat) != 0)
		{
			return FileUtil::FILE_NOT_FOUND;
		} // end if

		if (fileStat.st_mode & S_IFDIR)
			return FileUtil::DIRECTORY;
		else if (fileStat.st_mode & S_IFREG)
			return FileUtil::REGULAR_FILE;

		return FileUtil::FILE_NOT_FOUND;
	}

	std::string getFileExtension(const std::string& fileName)
	{
		std::string::size_type dot = fileName.find_last_of('.');
		std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
		if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) return std::string("");
		return std::string(fileName.begin() + dot + 1, fileName.end());
	}

	bool FileUtil::isAceptModelType(String filepath)
	{
		if (StringUtil::endsWith(filepath,"osg")
			||StringUtil::endsWith(filepath,"osgb")
			||StringUtil::endsWith(filepath,"osgt")
			||StringUtil::endsWith(filepath,"ive")
			||StringUtil::endsWith(filepath,"obj")
			||StringUtil::endsWith(filepath,"zip")
			||StringUtil::endsWith(filepath,"stl")
			||StringUtil::endsWith(filepath,"3ds")
			||StringUtil::endsWith(filepath,"dxf")
			)
		{
			return true;
		}

		return false;
	}	

	bool FileUtil::makeDirectory(String path)
	{
		if (path.empty())
		{
			return false;
		}

		struct stat64 stbuf;
		if (stat64(path.c_str(), &stbuf) == 0)
		{
			if (S_ISDIR(stbuf.st_mode))
				return true;
			else
			{
				return false;
			}
		}

		std::stack<std::string> paths;
		for (std::string dir = path;
			!dir.empty();
			dir = getFilePath(dir))
		{
			if (stat64(dir.c_str(), &stbuf) < 0)
			{
				switch (errno)
				{
				case ENOENT:
				case ENOTDIR:
					paths.push(dir);
					break;

				default:
					return false;
				}
			}
		}

		while (!paths.empty())
		{
			std::string dir = paths.top();

#if defined(WIN32)
			//catch drive name
			if (dir.size() == 2 && dir.c_str()[1] == ':') {
				paths.pop();
				continue;
			}
#endif

			if (mkdir(dir.c_str(), 0755) < 0)
			{
				// Only return an error if the directory actually doesn't exist.  It's possible that the directory was created
				// by another thread or process
				if (!fileExists(dir))
				{
					return false;
				}
			}
			paths.pop();
		}
		return true;
	}

	bool FileUtil::FileReName(String filename,String newFilename)
	{
		int ret = rename(filename.c_str(),newFilename.c_str());
		return ret == 0;
	}

	void FileUtil::FindFiles(const String strPath, StringVector &fileVector)
	{
		StringVector contents = getDirectoryContents(strPath);
		StringVector::iterator it = contents.begin();
		for (;it!= contents.end();it++)
		{
			String filepath = *it;

			if (filepath == ".." || filepath == ".")
			{
				continue;
			}

			filepath = strPath + "/" + filepath;

			if(FileUtil::REGULAR_FILE == fileType(filepath))
			{
				fileVector.push_back(filepath);
			}
			else if (FileUtil::DIRECTORY == fileType(filepath))
			{
				FindFiles(filepath,fileVector);
			}
		}
	}

	void FileUtil::FindModelFiles(const String strPath, StringVector &fileVector)
	{
		
	}

	FileUtil::Type FileUtil::FileType(String filename)
	{
		FileUtil::Type type = fileType(filename);
		if (type == FileUtil::REGULAR_FILE)
		{
			return REGULAR_FILE;
		}
		else if (type == FileUtil::DIRECTORY)
		{
			return DIRECTORY;
		}

		return FILE_NOT_FOUND;
	}

	int FileUtil::FileSize(String filename)
	{
		int size = -1;
		std::ifstream i(filename.c_str());
		if (i)
		{
			i.seekg(0,std::ios::end);
			size = i.tellg();
			i.close();
		}
		return size;
	}

	bool FileUtil::FileExist(String filename)
	{
		return fileType(filename) == REGULAR_FILE;
	}

	bool FileUtil::FileDelete(String filename)
	{
#if _WIN32
		return DeleteFile(filename.c_str()) == TRUE;
#else
		return remove(filename.c_str()) > 0;
#endif
	}

	bool FileUtil::DirDelete(String filename)
	{
#if _WIN32
		return RemoveDirectory(filename.c_str()) == TRUE;
#else
		return remove(filename.c_str()) > 0;
#endif
	}

	String FileUtil::getLowerCaseFileExtension(String filename)
	{
		String ext = getFileExtension(filename);
		StringUtil::toLowerCase(ext);
		return ext;
	}

	bool FileUtil::FileWrite(String filename,std::istream& fin)
	{
		std::ofstream ofs(filename.c_str(),std::ios::binary|std::ios::ate);
		if (ofs)
		{
			fin.seekg(0,std::ios::end);
			int size = fin.tellg();
			fin.seekg(0);
			char* buffer = new char[size];
			fin.read(buffer,size);
			ofs.write(buffer,size);
			CORE_SAFE_DELETE_ARRAY(buffer);
			ofs.close();
			return true;
		}

		return false;
	}
}