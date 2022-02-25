#ifndef _WATCHER_MANAGER_H__
#define _WATCHER_MANAGER_H__

#include "watcher.h"

using namespace OC;

class WatcherManager
{
public:
    WatcherManager();
    ~WatcherManager();
    void add(String path);
    void remove(String path);

    typedef std::map<String,osg::ref_ptr<watcher> > WatcherMap;
    WatcherMap mWatcherMap;
};


#endif