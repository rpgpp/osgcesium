#ifndef _CONVERT_PROCESS_WORKQUEUE_CLASS_H
#define _CONVERT_PROCESS_WORKQUEUE_CLASS_H

#include "OCMain/DefaultWorkQueueStandard.h"
#include "CesiumTool.h"

namespace OC
{
	namespace Cesium
	{
		class ConvertWorkqueue : public WorkQueue::ResponseHandler, public WorkQueue::RequestHandler
		{
		public:
			ConvertWorkqueue(int threadNum = 2);
			~ConvertWorkqueue(void);

			enum RequestType
			{
				RT_WriteDirectory,
				RT_Writeb3dm,
				RT_SplitAndWrite
			};

			struct TaskRequest
			{
				RequestType				type;
				String srcDir;
				String destDir;
				String file;
				int code;
				osg::ref_ptr<osg::Node> rootNode;
				ObliqueConverter* converter;
				osg::ref_ptr<OC::Cesium::Tile> parentTile;
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
			int finished = 0;
			WorkQueue* mWorkQueue;
		};
	}
}


#endif