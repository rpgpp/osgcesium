#include "ConvWorkqueue.h"
#include "OConvMapCacher.h"

OConvWorkqueue::OConvWorkqueue(int threadNum)
{
	mWorkQueue = new DefaultWorkQueue("Task");
	((DefaultWorkQueue*)mWorkQueue)->setWorkerThreadCount(threadNum);
	mWorkQueue->startup();
	mWorkQueueChannel = mWorkQueue->getChannel("Task");
	mWorkQueue->addResponseHandler(mWorkQueueChannel, this);
	mWorkQueue->addRequestHandler(mWorkQueueChannel, this);
}


OConvWorkqueue::~OConvWorkqueue(void)
{
	shutdown();
	CORE_SAFE_DELETE(mWorkQueue);
}

void OConvWorkqueue::shutdown()
{
	mWorkQueue->abortRequestsByChannel(mWorkQueueChannel);
	mWorkQueue->removeRequestHandler(mWorkQueueChannel, this);
	mWorkQueue->removeResponseHandler(mWorkQueueChannel, this);
}

void OConvWorkqueue::addRequest(TaskRequest request)
{
	taskCount++;
	mWorkQueue->addRequest(mWorkQueueChannel, request.type, Any(request));
}

void OConvWorkqueue::handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
{
	taskCount--;
	if (res->getRequest()->getAborted() || !res->succeeded())
	{
		return;
	}
}

WorkQueue::Response* OConvWorkqueue::handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
{
	if (req->getAborted())
	{
		return new WorkQueue::Response(req, false, req->getData());
	}

	bool handle_succeed = false;

	TaskRequest& anyRequest = OC::any_cast<TaskRequest>(req->getData());
	
	if (anyRequest.type == RT_MapCacherCes)
	{
		OConvMapCacher cacher;
		cacher.requestCes(anyRequest.X, anyRequest.Y, anyRequest.Z, anyRequest.L);
	}
	else if (anyRequest.type == RT_MapCacherTDT)
	{
		OConvMapCacher cacher;
		cacher.requestTDT(anyRequest.param, anyRequest.X, anyRequest.Y, anyRequest.Z, anyRequest.L);
	}

	handle_succeed = true;

	return new WorkQueue::Response(req, handle_succeed, req->getData());
}