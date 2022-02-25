#include "TaskWorkQueue.h"
#include "watcher.h"
#include "Tool.h"
#include "fetcher.h"

namespace OC
{
    TaskWorkQueue* single_instance_TaskWorkQueue = NULL;
    TaskWorkQueue& TaskWorkQueue::instance(int threadNum)
    {
        if(single_instance_TaskWorkQueue == NULL)
        {
            single_instance_TaskWorkQueue = new TaskWorkQueue(threadNum);
        }

        return *single_instance_TaskWorkQueue;
    }

    TaskWorkQueue::TaskWorkQueue(int threadNum)
    {
        mWorkQueue = new DefaultWorkQueue("Task");
        ((DefaultWorkQueue*)mWorkQueue)->setWorkerThreadCount(threadNum);
        mWorkQueue->startup();
        mWorkQueueChannel = mWorkQueue->getChannel("Task");
        mWorkQueue->addResponseHandler(mWorkQueueChannel, this);
        mWorkQueue->addRequestHandler(mWorkQueueChannel, this);
    }

    TaskWorkQueue::~TaskWorkQueue()
    {
        shutdown();
        CORE_SAFE_DELETE(mWorkQueue);
    }

    void TaskWorkQueue::shutdown()
    {
        mWorkQueue->abortRequestsByChannel(mWorkQueueChannel);
        mWorkQueue->removeRequestHandler(mWorkQueueChannel, this);
        mWorkQueue->removeResponseHandler(mWorkQueueChannel, this);
    }

    void TaskWorkQueue::addRequest(TaskRequest request)
    {
        mWorkQueue->addRequest(mWorkQueueChannel, request.type, Any(request));
    }

    void TaskWorkQueue::handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
    {
        if (res->getRequest()->getAborted() || !res->succeeded())
        {
            return;
        }
    }

    WorkQueue::Response* TaskWorkQueue::handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
    {
        if (req->getAborted())
        {
            return new WorkQueue::Response(req, false, req->getData());
        }
        bool handle_succeed = false;

	    const TaskRequest& anyRequest = OC::any_cast<TaskRequest>(req->getData());
        String dir = anyRequest.dir;

        if(anyRequest.type == RT_WatchDirectory)
        {
            mWatcherManager.add(dir);
        }
        else if(anyRequest.type == RT_NotifyCreate)
        {
            if(FileUtil::FileType(dir) == FileUtil::DIRECTORY)
            {
                mWatcherManager.add(dir);
            }
        }
        else if(anyRequest.type == RT_NotifyEndWrite)
        {
            if(FileUtil::FileType(dir) == FileUtil::REGULAR_FILE)
            {
                //handler caches
                handleFile(dir,anyRequest.type);
            }
        }
        else if(anyRequest.type == RT_NotifyMove)
        {
            mWatcherManager.remove(dir);
        }
        else if (anyRequest.type == RT_test)
        {
            test();
        }
        else if (anyRequest.type == RT_FetchDirectory)
        {
            CFetcher fetcher(dir);
            fetcher.start();
        }
        
	    handle_succeed = true;
        return new WorkQueue::Response(req, handle_succeed, req->getData());
    }
}

