#include "OCMain/Environment.h"
#include "OCMain/ConfigManager.h"
#include "OCMain/DefaultWorkQueueStandard.h"

using namespace OC;

class OConvWorkqueue : public WorkQueue::ResponseHandler, public WorkQueue::RequestHandler
{
public:
	OConvWorkqueue(int threadNum = 2);
	~OConvWorkqueue(void);

	enum RequestType
	{
		RT_MapCacherCes,
		RT_MapCacherTDT,
	};

	struct TaskRequest
	{
		RequestType				type;
		int X, Y, Z, L;
		String param;
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
	int taskCount = 0;
};
