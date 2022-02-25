#ifndef _WATCHER_H__
#define _WATCHER_H__

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <iostream>
#include "OCMain/osg.h"
#include "OCMain/Environment.h"

class watcher : public osg::Referenced
{
public:
    watcher();
    ~watcher();
    int createTask(std::string path);
    int fd;
    int wd;
    std::string _path;
};

#endif