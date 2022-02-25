#include "fetcher.h"
#include "OCMain/ConfigManager.h"
#include "Tool.h"

static String TemplateSWAN_NAME = "Z_OTHE_RADAMOSAIC_{0}00.bin";
static String TemplateCINR_NAME = "Z_RADR_I_{0}_{1}_O_DOR_{2}_CAP_FMT00.bin";

OpenThreads::ReentrantMutex CFetcher::mutex;
time_t CFetcher::mStaticLastClearTick = 0;

CFetcher::CFetcher(String dir)
    :monitor_tick(0)
    ,mRunning(true)
    ,mInterval(60)
    ,mMonitorDir(dir)
{
    CORE_LOCK_MUTEX(mutex)
    mInterval = Singleton(ConfigManager).getIntValue("mInterval", 60);
}

CFetcher::~CFetcher()
{
}

time_t getTodayTick()
{
    time_t t = time(0);
    return t - t % (3600 * 24) - 3600 * 8;
}

String getTodayStr()
{
    time_t t0 = getTodayTick();
    char tmp[64];
    strftime(tmp, sizeof(tmp),"%Y%m%d", localtime(&t0));
    return tmp;
}

void CFetcher::clear()
{
    CORE_LOCK_MUTEX(mutex)
    time_t today = getTodayTick();
    if (mStaticLastClearTick == today)
    {
        return;
    }
    mStaticLastClearTick = today;
    unsigned int count = 0;
    String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
    time_t todayTick = getTodayTick();
    StringVector files;
    FileUtil::FindFiles(cachedir, files);
    for (StringVector::iterator it = files.begin(); it != files.end(); it++)
    {
        String timeStr;
        String filename = *it;
        StringVector sv = StringUtil::split(osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(filename)), "_");
        if (sv.size() == 4)
        {
            timeStr = sv[3];
        }
        else if (sv.size() == 10)
        {
            timeStr = sv[4];
        }
        if (timeStr.length() == 14)
        {
            String year = timeStr.substr(0, 4);
            String month = timeStr.substr(4, 2);
            String day = timeStr.substr(6, 2);
            struct tm ti;
            memset(&ti, 0, sizeof(ti));
            ti.tm_year = StringConverter::parseInt(year) - 1900;
            ti.tm_mon = StringConverter::parseInt(month) - 1;
            ti.tm_mday = StringConverter::parseInt(day);
            ti.tm_hour = 0;
            ti.tm_min = 0;
            ti.tm_sec = 0;
            time_t file_tick = mktime(&ti);
            if (file_tick < todayTick)
            {
                FileUtil::FileDelete(filename);
                count++;
            }
        }
    }
    std::cout << "clear count: " << count << std::endl;
}

void CFetcher::start()
{
    String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
    String monitordir;
    size_t cachedCount = 0;
    std::map<String,int> errorMap;
    while (mRunning)
    {
        String today = getTodayStr();
        if (mMonitorDay != today)
        {
            mMonitorDay = today;
            // clear caches
            clear();
            errorMap.clear();
            cachedCount = 0;

            // change monitor folder
            monitordir = mMonitorDir + "/" + today;
            std::cout<< "fetching dir:" << monitordir << std::endl;
        }

        osgDB::DirectoryContents files = osgDB::getSortedDirectoryContents(monitordir);
        size_t size = files.size();
        //generate new
        for (; cachedCount< size; cachedCount++)
        {
            String content = files[cachedCount];
            if(content == "." || content == "..")
            {
                continue;
            }

            String filename = monitordir + "/" + content;

            int filesize = FileUtil::FileSize(filename);
            if (filesize < 10 && errorMap[filename] < 24)
            {
                cachedCount--;
                errorMap[filename]++;
                break;
            }

            String file = osgDB::getSimpleFileName(filename);
            String ext = osgDB::getLowerCaseFileExtension(file);
            if (ext == "bz2")
            {
                StringUtil::toUpperCase(file);
                String varname = StringUtil::startsWith(file, "Z_OTHE_RADAMOSAIC_", false) ? "DBZ" : "EF6A0A74";
                String cachefilename = cachedir + StringUtil::replaceAll(file, ".BZ2", "." + varname + ".w3dm");   
                if (!FileUtil::FileExist(cachefilename))
                {
                    convertVolume(filename,cachefilename,varname);
                }
            }
        }
        
        OpenThreads::Thread::microSleep(mInterval * 1000 * 1000);
    }
}

void CFetcher::stop()
{

}