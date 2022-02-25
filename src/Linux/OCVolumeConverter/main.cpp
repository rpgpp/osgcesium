#include "Tool.h"
#include "OCMain/Environment.h"
#include "OCMain/ConfigManager.h"

String getTodayStr2(time_t t0)
{
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S", localtime(&t0));
	return tmp;

}
int main(int argc, char** argv)
{
	#if 0
	{
		{
			StringVector sv;
			FileUtil::FindFiles("E:\\2021-swan", sv);
			for (auto i : sv)
			{
				std::string filename = i;
				StringUtil::toUpperCase(filename);
				std::string outfilename = StringUtil::replaceAll(filename, ".BZ2", ".DBZ.w3dm");

				StringVector sv = StringUtil::split(osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(filename)), "_");
				String timeStr = sv[3];
				String year = timeStr.substr(0, 4);
				String month = timeStr.substr(4, 2);
				String day = timeStr.substr(6, 2);
				String hour = timeStr.substr(8, 2);
				String minute = timeStr.substr(10, 2);
				String sec = timeStr.substr(12, 2);
				struct tm ti;
				memset(&ti, 0, sizeof(ti));
				ti.tm_year = StringConverter::parseInt(year) - 1900;
				ti.tm_mon = StringConverter::parseInt(month) - 1;
				ti.tm_mday = StringConverter::parseInt(day);
				ti.tm_hour = StringConverter::parseInt(hour);
				ti.tm_min = StringConverter::parseInt(minute);
				ti.tm_sec = StringConverter::parseInt(sec);
				time_t file_tick = mktime(&ti);
				file_tick += (3600 * 8);
				String time2 = getTodayStr2(file_tick);
				outfilename = StringUtil::replaceAll(outfilename,timeStr, time2);
				convertVolume(filename, outfilename, "DBZ");
			}
		}
	}

	return 0;
	#endif


	String currentPath = StringUtil::replaceAll(argv[0], "OCVolumeConverter", "");
	currentPath = StringUtil::replaceAll(currentPath, "d.exe", "");
	currentPath = StringUtil::replaceAll(currentPath, ".exe", "");
	std::cout<<"WorkSpace:" << currentPath<<std::endl;
	new Environment(0x6C6777);
	Singleton(Environment).setAppDir(currentPath);
	new ConfigManager(currentPath  + "config.ini");
	String command = Singleton(ConfigManager).getStringValue("command");
	if( command == "watch")
	{
		std::cout<<"Command:" << command<<std::endl;
		String watchdir = Singleton(ConfigManager).getStringValue("watchdir");
		int threadnum = Singleton(ConfigManager).getIntValue("threadnum",5);

		String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
		if(cachedir.empty())
		{
			std::cout<<"cachedir is empty"<<std::endl;
			return -1;
		}
    	osgDB::makeDirectory(cachedir);
		if(!StringUtil::endsWith(cachedir,"/"))
		{
			cachedir += "/";
			Singleton(ConfigManager).setConfig("cachedir",cachedir);
		}
		
		StringVector sv = StringUtil::split(watchdir);
		TaskWorkQueue::instance(threadnum);
		if(sv.size() == 0 )
		{
			std::cout<<"no watch dir"<<std::endl;
			return -1;
		}

		for(StringVector::iterator it = sv.begin();it!=sv.end();it++)
		{
			String dir = *it;
			TaskWorkQueue::TaskRequest request;
			request.type = TaskWorkQueue::RT_WatchDirectory;
			request.dir = dir;
			TaskWorkQueue::instance().addRequest(request);
			OpenThreads::Thread::microSleep(100 * 1000);

			osgDB::DirectoryContents contents = osgDB::getDirectoryContents(dir);
			osgDB::DirectoryContents::iterator it2 = contents.begin();
			for (;it2!= contents.end();it2++)
			{
				String filepath = *it2;

				if (filepath == ".." || filepath == ".")
				{
					continue;
				}

				filepath = dir + "/" + filepath;
			    if (osgDB::DIRECTORY == osgDB::fileType(filepath))
				{
					request.dir = filepath;
					TaskWorkQueue::instance().addRequest(request);
				}
			}
		}

		time_t oldday = 0;
		while (true)
		{
			TaskWorkQueue::instance().mWorkQueue->processResponses();
			time_t today = tickTheDayBefore(0);
			if(oldday != today)
			{
            	std::cout<<"Clear caches before the day "<<parseTime(today)<<std::endl;
				oldday = today;
				clearCacher(today);
			}
			OpenThreads::Thread::microSleep(60000 * 1000);
		}
	}
	else if (command == "fetch")
	{
		std::cout << "Command:" << command << std::endl;
		String watchdir = Singleton(ConfigManager).getStringValue("watchdir");
		int threadnum = Singleton(ConfigManager).getIntValue("threadnum", 5);
		String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
		if (cachedir.empty())
		{
			std::cout << "cachedir is empty" << std::endl;
			return -1;
		}
		osgDB::makeDirectory(cachedir);
		if (!StringUtil::endsWith(cachedir, "/"))
		{
			cachedir += "/";
			Singleton(ConfigManager).setConfig("cachedir", cachedir);
		}

		StringVector sv = StringUtil::split(watchdir);
		TaskWorkQueue::instance(threadnum);
		if (sv.size() == 0)
		{
			std::cout << "no watch dir" << std::endl;
			return -1;
		}

		for (StringVector::iterator it = sv.begin(); it != sv.end(); it++)
		{
			String dir = *it;
			TaskWorkQueue::TaskRequest request;
			request.type = TaskWorkQueue::RT_FetchDirectory;
			request.dir = dir;
			TaskWorkQueue::instance().addRequest(request);
		}

		while (true)
		{
			TaskWorkQueue::instance().mWorkQueue->processResponses();
			OpenThreads::Thread::microSleep(60000 * 1000);
		}
	}

	if (argc == 4)
	{
		std::string filename = argv[1];
		std::string outfilename = argv[2];
		std::string varname = argv[3];
		convertVolume(filename,outfilename,varname);
		return 0;
	}
	else if (argc == 2)
	{
		std::string command = argv[1];
		if (command == "test")
		{
			
		}
	}
	std::cout<<"Sample Command: OCVolumeConverter.exe in_file out_file varname"<<std::endl;
	return -1;
}