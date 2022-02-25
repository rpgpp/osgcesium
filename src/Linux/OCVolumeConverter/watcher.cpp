#include "watcher.h"
#include "TaskWorkQueue.h"

using namespace OC;

watcher::watcher()
    :fd(-1)
    , wd(-1)
{

}

#if CORE_PLATFORM == CORE_PLATFORM_LINUX
#include <sys/inotify.h>  
#include <unistd.h>  


watcher::~watcher()
{
    if (fd > 0 && wd > 0)
    {
        inotify_rm_watch(fd, wd);
    }
}


int watcher::createTask(std::string path)
{
    _path = path;
    if(!StringUtil::endsWith(_path,"/"))
    {
        _path += "/";
    }

    int len;
    int nread;
    char buf[BUFSIZ];
    struct inotify_event *event;
    int i;

    fd = inotify_init();
    if (fd < 0)
    {
        fprintf(stderr, "inotify_init failed\n");
        return -1;
    }

    wd = inotify_add_watch(fd, path.c_str(), IN_CREATE | IN_CLOSE_WRITE | IN_MOVE);
    if (wd < 0)
    {
        fprintf(stderr, "inotify_add_watch %s failed\n", path.c_str());
        return -1;
    }

    buf[sizeof(buf) - 1] = 0;
    while ((len = read(fd, buf, sizeof(buf) - 1)) > 0)
    {
        nread = 0;
        while (len > 0)
        {
            event = (struct inotify_event *)&buf[nread];
            //std::cout<<hex<<event->mask;
            //std::cout<< "|"<< event->name << std::endl;
            if (event->mask & IN_CLOSE)
            {
                TaskWorkQueue::TaskRequest request;
			    request.type = TaskWorkQueue::RT_NotifyEndWrite;
			    request.dir = _path + event->name;
                // TaskWorkQueue::instance().addRequest(request);
            }
            else if (event->mask & IN_CREATE)
            {
                std::cout<< "IN_CREATE|"<< event->name << std::endl;
            }
            else if (event->mask & IN_MODIFY)
            {
                std::cout<< "IN_MODIFY|"<< event->name << std::endl;
            }
            else if (event->mask & IN_ATTRIB)
            {
                std::cout<< "IN_ATTRIB|"<< event->name << std::endl;
            }
            else if (event->mask & IN_MOVE_SELF)
            {
                std::cout<< "IN_MOVE_SELF|"<< event->name << std::endl;
            }
            else if (event->mask & IN_MOVE)
            {
                std::cout<< "IN_MOVE|"<< event->name << std::endl;
            }
            else if (event->mask & IN_CLOSE_WRITE)
            {
                std::cout<< "IN_CLOSE_WRITE|"<< event->name << std::endl;
            }
            else if (event->mask & (IN_DELETE 
            | IN_DELETE_SELF
            | IN_MOVED_TO
            ))
            {

                TaskWorkQueue::TaskRequest request;
			    request.type = TaskWorkQueue::RT_NotifyMove;
			    request.dir = _path + event->name;
                TaskWorkQueue::instance().addRequest(request);
            }
            else
            {
            }
            nread = nread + sizeof(struct inotify_event) + event->len;
            len = len - sizeof(struct inotify_event) - event->len;
        }
    }

    return fd;
}

#else

watcher::~watcher()
{

}

int watcher::createTask(std::string path)
{
    return 0;
}
#endif
