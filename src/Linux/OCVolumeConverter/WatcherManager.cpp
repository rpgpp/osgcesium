#include "WatcherManager.h"

WatcherManager::WatcherManager()
{
    
}

WatcherManager::~WatcherManager()
{

}

void WatcherManager::add(String path)
{
    if(mWatcherMap.find(path) != mWatcherMap.end())
    {
        std::cout<<"Already watching:" << path << std::endl;
        return;
    }
    std::cout<<"start watching:" << path << std::endl;

    watcher* wat = mWatcherMap[path] = new watcher;
    wat->createTask(path);
}

void WatcherManager::remove(String path)
{
    if(mWatcherMap.find(path) != mWatcherMap.end())
    {
        std::cout<<"remove watching:" << path << std::endl;
        mWatcherMap.erase(path);
    }
}

