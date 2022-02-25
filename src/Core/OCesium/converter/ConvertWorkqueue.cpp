#include "ConvertWorkqueue.h"
#include "ObliqueConverter.h"

using namespace OC;
using namespace OC::Cesium;

ConvertWorkqueue::ConvertWorkqueue(int threadNum)
{
	mWorkQueue = new DefaultWorkQueue("Task");
	((DefaultWorkQueue*)mWorkQueue)->setWorkerThreadCount(threadNum);
	mWorkQueue->startup();
	mWorkQueueChannel = mWorkQueue->getChannel("Task");
	mWorkQueue->addResponseHandler(mWorkQueueChannel, this);
	mWorkQueue->addRequestHandler(mWorkQueueChannel, this);
}


ConvertWorkqueue::~ConvertWorkqueue(void)
{
	shutdown();
	CORE_SAFE_DELETE(mWorkQueue);
}

void ConvertWorkqueue::shutdown()
{
	mWorkQueue->abortRequestsByChannel(mWorkQueueChannel);
	mWorkQueue->removeRequestHandler(mWorkQueueChannel, this);
	mWorkQueue->removeResponseHandler(mWorkQueueChannel, this);
}

void ConvertWorkqueue::addRequest(TaskRequest request)
{
	finished++;
	mWorkQueue->addRequest(mWorkQueueChannel, request.type, Any(request));
}

void ConvertWorkqueue::handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
{
	finished--;
	if (res->getRequest()->getAborted() || !res->succeeded())
	{
		return;
	}
	//TaskRequest anyRequest = OC::any_cast<TaskRequest>(res->getData());
	//if (RT_WriteDirectory == anyRequest.type)
	//{

	//}
}

WorkQueue::Response* ConvertWorkqueue::handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
{
	if (req->getAborted())
	{
		return new WorkQueue::Response(req, false, req->getData());
	}

	bool handle_succeed = false;

	TaskRequest& anyRequest = OC::any_cast<TaskRequest>(req->getData());

	if (anyRequest.type == RT_WriteDirectory)
	{
		String srcDir = anyRequest.srcDir;
		String destDir = anyRequest.destDir;
		String file = anyRequest.file;
		anyRequest.converter->convert(srcDir, destDir, file, 0, anyRequest.parentTile, anyRequest.rootNode);
	}
	else if (anyRequest.type == RT_Writeb3dm)
	{
		Cesium::B3dmWriter writer;
		writer.getGltfObject()->convert(anyRequest.rootNode);
		writer.writeToFile(anyRequest.file);
		double fileSize = writer.bytesLength() / 1048576.0;
		std::cout << anyRequest.file << ":" << StringConverter::formatDoubletoString(fileSize) << std::endl;
		anyRequest.rootNode->asGroup()->removeChild(0, anyRequest.rootNode->asGroup()->getNumChildren());
	}
	else if (anyRequest.type == RT_SplitAndWrite)
	{
		anyRequest.converter->split(anyRequest.rootNode->asGroup(), anyRequest.parentTile, false, 1, anyRequest.destDir, anyRequest.code);
		//anyRequest.rootNode->asGroup()->removeChild(0, anyRequest.rootNode->asGroup()->getNumChildren());
	}
	

	handle_succeed = true;

	return new WorkQueue::Response(req, handle_succeed, req->getData());
}