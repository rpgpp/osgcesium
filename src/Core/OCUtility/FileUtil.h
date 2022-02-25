#ifndef _FILE_UTIL_CLASS_H_
#define _FILE_UTIL_CLASS_H_

#include "StringUtil.h"

namespace OC
{
	class _UtilityExport FileUtil
	{
	public:
		enum Type
		{
			FILE_NOT_FOUND,
			REGULAR_FILE,
			DIRECTORY
		};

		/*
		* return 0 File 1 Folder -1 NoteFound
		*/
		static Type FileType(String filename);

		static void FindFiles(const String strPath, StringVector &fileVector);

		static void FindModelFiles(const String strPath, StringVector &fileVector);

		static bool FileExist(String filename);

		static int FileSize(String filename);

		static bool FileDelete(String filename);

		static bool DirDelete(String filename);
		
		static bool FileWrite(String filename,std::istream& fin);

		static String getLowerCaseFileExtension(String filename);

		static bool isAceptModelType(String filename);

		static bool makeDirectory(String dir);

		static bool FileReName(String filename,String newFilename);
	};
}

#endif