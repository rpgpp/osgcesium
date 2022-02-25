#ifndef _TASK_WORKQUE_H__
#define _TASK_WORKQUE_H__
#include "OCMain/DefaultWorkQueueStandard.h"
#include "WatcherManager.h"

namespace OC
{
    class TaskWorkQueue : public WorkQueue::ResponseHandler, public WorkQueue::RequestHandler
    {
    private:
                TaskWorkQueue(int threadNum = 2);
    public:
                ~TaskWorkQueue();
                static TaskWorkQueue& instance(int threadNum = 2);
    enum RequestType
                {
                    RT_WatchDirectory,
                    RT_FetchDirectory,
                    RT_NotifyCreate,
                    RT_NotifyEndWrite,
                    RT_NotifyMove,
                    RT_test
                };

                struct TaskRequest
                {
                    RequestType			   type;
                    String                      dir;
                    friend std::ostream& operator<<(std::ostream& o, const TaskRequest& r)
                    {
                        (void)r; return o;
                    }
                };

                void shutdown();
                int mWorkQueueChannel;
                virtual void handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ);
                virtual WorkQueue::Response* handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ);
                void addRequest(TaskRequest request);
                WorkQueue* mWorkQueue;
                WatcherManager mWatcherManager;
    };
}

#endif