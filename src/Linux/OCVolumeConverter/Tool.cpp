#include "Tool.h"
#include <osgDB/FileNameUtils>
#include "OCVolume/Volume.h"
#include "OCMain/DataStream.h"
#include "OCMain/ConfigManager.h"

using namespace OC;
using namespace OC::Volume;

int convertVolume(String filename,String outfilename,String varname)
{
    CVolume v;
    osg::ref_ptr<Volume::CVolumeObject> volumeObj = v.convert(filename, varname);
    if (volumeObj.valid())
    {
        volumeObj->writeToW3DM(outfilename);
        return 0;
    }

    return -1;
}


time_t tickTheDayBefore(time_t daybefore)
{
	time_t t = time(0);
	return  t - t % (3600 * 24)  - 3600 * 8 - 3600 * 8 * daybefore;
}

String parseTime(time_t t)
{
	char tmp[64];
    strftime( tmp, sizeof(tmp), "%Y-%m-%d",localtime(&t) );
	return tmp;
}

int clearCacher(time_t today)
{
     String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
    if(cachedir.empty())
    {
        std::cout<<"cachedir is empty"<<std::endl;
        return 0;
    }
    
    StringVector files;
    FileUtil::FindFiles(cachedir,files);
    for(StringVector::iterator it = files.begin();it!=files.end();it++)
    {
        String timeStr;
        String filename = *it;
        StringVector sv = StringUtil::split(osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(filename)),"_");
        if(sv.size() == 4)
        {
            timeStr = sv[3];
        }
        else if(sv.size() == 9)
        {
            timeStr = sv[4];
        }
        if(timeStr.length() == 14)
        {
            String year = timeStr.substr(0,4);
            String month = timeStr.substr(4,2);
            String day = timeStr.substr(6,2);
            struct tm ti;
            memset(&ti, 0, sizeof(ti));
            ti.tm_year = StringConverter::parseInt(year) - 1900;
            ti.tm_mon = StringConverter::parseInt(month) - 1;
            ti.tm_mday = StringConverter::parseInt(day);
            ti.tm_hour = 0;
            ti.tm_min = 0;
            ti.tm_sec = 0;
            long file_tick = mktime(&ti);
            //std::cout<<parseTime(file_tick)<<"/" << file_tick<<std::endl;
            if(file_tick < today)
            {
                //
                std::cout<<"del " << filename << std::endl;
                FileUtil::FileDelete(filename);
            }
        }
    }
    return 0;
}

void test()
{
    std::cout<<"test() in"<<std::endl;
    String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
    String infile = "/home/liguowu/projects/Data/Z_OTHE_RADAMOSAIC_20200718154800.bin.bz2";
    String outfile = "/home/liguowu/projects/bin/20211127/Z_OTHE_RADAMOSAIC_20200718154800.bin.bz2";
    if(FileUtil::FileDelete("/home/liguowu/projects/bin/20211127"))
    {
        std::cout<<"remove /home/liguowu/projects/bin/20211127."<<std::endl;
    }
    osgDB::makeDirectory("/home/liguowu/projects/bin/20211127");
    MemoryDataStreamPtr memDataStream;
    osgDB::ifstream ifs(infile.c_str(),std::ios::binary);
    if(ifs)
    {
        DataStreamPtr dataStream(new FileStreamDataStream(&ifs,false));
        size_t size = dataStream->size();
        memDataStream = MemoryDataStreamPtr(new MemoryDataStream(size));
        dataStream->read(memDataStream->getPtr(),size);
        ifs.close();
    }
    else
    {
        std::cout<<"test no in file."<<std::endl;
    }

    if(!memDataStream.isNull())
    {
        size_t size = memDataStream->size();
        ofstream ofs(outfile.c_str(),std::ios::binary);
        if(ofs)
        {
            std::cout<<"test() sleep 1000ms."<<std::endl;
            OpenThreads::Thread::microSleep(1000 * 1000);
            ofs.write((const char*)memDataStream->getPtr(),size);
            ofs.close();
            std::cout<<"test() sleep end."<<std::endl;
        }
    }
    std::cout<<"test() out"<<std::endl;
}


#if CORE_PLATFORM == CORE_PLATFORM_LINUX
int handleFile(String filename,OC::TaskWorkQueue::RequestType type )
{
        if(FileUtil::FileType(filename) == FileUtil::REGULAR_FILE)
        {
            int filesize = FileUtil::FileSize(filename);
            if(filesize < 10)
            {
                return -1;
            }
            String file = osgDB::getSimpleFileName(filename);
            String ext = osgDB::getLowerCaseFileExtension(file);
            if(ext == "bz2")
            {
                String cachedir = Singleton(ConfigManager).getStringValue("cachedir");
                if(cachedir.empty())
                {
                    std::cout<<"cachedir is empty"<<std::endl;
                    return -1;
                }
                osgDB::makeDirectory(cachedir);
                StringUtil::toUpperCase(file);
                
                String varname = StringUtil::startsWith(file,"Z_OTHE_RADAMOSAIC_",false) ? "DBZ" : "EF6A0A74";
                String cachefilename = cachedir +  StringUtil::replaceAll(file,".BZ2","." + varname+ ".w3dm");
                if(!FileUtil::FileExist(cachefilename))
                {
                    std::cout<< "converting:"<<filename<< " size{" <<filesize<<"}"<<std::endl;
                    std::ofstream ofs(cachefilename.c_str(),std::ios::binary);
                    ofs.close();
                }
            }
        }

        if(type == OC::TaskWorkQueue::RT_NotifyMove)
        {
            std::cout<<"NotifyMove:" << filename << std::endl;
        }
        else if(type == OC::TaskWorkQueue::RT_NotifyCreate)
        {
            std::cout<<"NotifyCreate:" << filename << std::endl;
        }
}
#else
int handleFile(String filename, OC::TaskWorkQueue::RequestType type)
{
    return -1;
}
#endif
