#include "WorkQueue.h"

namespace OC {

	//---------------------------------------------------------------------
	uint16 WorkQueue::getChannel(const String& channelName)
	{
		CORE_LOCK_MUTEX(mChannelMapMutex)

		ChannelMap::iterator i = mChannelMap.find(channelName);
		if (i == mChannelMap.end())
		{
			i = mChannelMap.insert(ChannelMap::value_type(channelName, mNextChannel++)).first;
		}
		return i->second;
	}
	//---------------------------------------------------------------------
	WorkQueue::Request::Request(uint16 channel, uint16 rtype, const Any& rData, uint8 retry, RequestID rid)
		: mChannel(channel), mType(rtype), mData(rData), mRetryCount(retry), mID(rid), mAborted(false)
	{

	}
	//---------------------------------------------------------------------
	WorkQueue::Request::~Request()
	{

	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	WorkQueue::Response::Response(const Request* rq, bool success, const Any& data, const String& msg)
		: mRequest(rq), mSuccess(success), mMessages(msg), mData(data)
	{
		
	}
	//---------------------------------------------------------------------
	WorkQueue::Response::~Response()
	{
		CORE_DELETE mRequest;
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	DefaultWorkQueueBase::DefaultWorkQueueBase(const String& name)
		: mName(name)
		, mWorkerThreadCount(1)
		, mWorkerRenderSystemAccess(false)
		, mIsRunning(false)
		, mResposeTimeLimitMS(8)
		, mWorkerFunc(0)
		, mRequestCount(0)
		, mPaused(false)
		, mAcceptRequests(true)
	{
	}
	//---------------------------------------------------------------------
	const String& DefaultWorkQueueBase::getName() const
	{
		return mName;
	}
	//---------------------------------------------------------------------
	size_t DefaultWorkQueueBase::getWorkerThreadCount() const
	{
		return mWorkerThreadCount;
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::setWorkerThreadCount(size_t c)
	{
		mWorkerThreadCount = c;
	}
	//---------------------------------------------------------------------
	bool DefaultWorkQueueBase::getWorkersCanAccessRenderSystem() const
	{
		return mWorkerRenderSystemAccess;
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::setWorkersCanAccessRenderSystem(bool access)
	{
		mWorkerRenderSystemAccess = access;
	}
	//---------------------------------------------------------------------
	DefaultWorkQueueBase::~DefaultWorkQueueBase()
	{
		//shutdown(); // can't call here; abstract function

		for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
		{
			CORE_DELETE (*i);
		}
		mRequestQueue.clear();

		for (ResponseQueue::iterator i = mResponseQueue.begin(); i != mResponseQueue.end(); ++i)
		{
			CORE_DELETE (*i);
		}
		mResponseQueue.clear();
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::addRequestHandler(uint16 channel, RequestHandler* rh)
	{
		CORE_LOCK_RW_MUTEX_WRITE(mRequestHandlerMutex);

		RequestHandlerListByChannel::iterator i = mRequestHandlers.find(channel);
		if (i == mRequestHandlers.end())
			i = mRequestHandlers.insert(RequestHandlerListByChannel::value_type(channel, RequestHandlerList())).first;

		RequestHandlerList& handlers = i->second;
		bool duplicate = false;
		for (RequestHandlerList::iterator j = handlers.begin(); j != handlers.end(); ++j)
		{
			if ((*j)->getHandler() == rh)
			{
				duplicate = true;
				break;
			}
		}
		if (!duplicate)
			handlers.push_back(RequestHandlerHolderPtr(CORE_NEW RequestHandlerHolder(rh)));

	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::removeRequestHandler(uint16 channel, RequestHandler* rh)
	{
		CORE_LOCK_RW_MUTEX_WRITE(mRequestHandlerMutex);

		RequestHandlerListByChannel::iterator i = mRequestHandlers.find(channel);
		if (i != mRequestHandlers.end())
		{
			RequestHandlerList& handlers = i->second;
			for (RequestHandlerList::iterator j = handlers.begin(); j != handlers.end(); ++j)
			{
				if ((*j)->getHandler() == rh)
				{
					// Disconnect - this will make it safe across copies of the list
					// this is threadsafe and will wait for existing processes to finish
					(*j)->disconnectHandler();
					handlers.erase(j);	
					break;
				}
			}

		}

	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::addResponseHandler(uint16 channel, ResponseHandler* rh)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(channel);
		if (i == mResponseHandlers.end())
			i = mResponseHandlers.insert(ResponseHandlerListByChannel::value_type(channel, ResponseHandlerList())).first;

		ResponseHandlerList& handlers = i->second;
		if (std::find(handlers.begin(), handlers.end(), rh) == handlers.end())
			handlers.push_back(rh);
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::removeResponseHandler(uint16 channel, ResponseHandler* rh)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(channel);
		if (i != mResponseHandlers.end())
		{
			ResponseHandlerList& handlers = i->second;
			ResponseHandlerList::iterator j = std::find(
				handlers.begin(), handlers.end(), rh);
			if (j != handlers.end())
				handlers.erase(j);

		}
	}
	//---------------------------------------------------------------------
	WorkQueue::RequestID DefaultWorkQueueBase::addRequest(uint16 channel, uint16 requestType, 
		const Any& rData, uint8 retryCount, bool forceSynchronous)
	{
		Request* req = 0;
		RequestID rid = 0;

		{
			// lock to acquire rid and push request to the queue
			CORE_LOCK_MUTEX(mRequestMutex)

			if (!mAcceptRequests || mShuttingDown)
				return 0;

			rid = ++mRequestCount;
			req = CORE_NEW Request(channel, requestType, rData, retryCount, rid);

			if (!forceSynchronous)
			{
				mRequestQueue.push_back(req);
				notifyWorkers();
				return rid;
			}
		}
		

		processRequestResponse(req, true);

		return rid;

	}

	WorkQueue::RequestID DefaultWorkQueueBase::addRequestFront(uint16 channel, uint16 requestType, 
		const Any& rData, uint8 retryCount, bool forceSynchronous)
	{
		Request* req = 0;
		RequestID rid = 0;

		{
			// lock to acquire rid and push request to the queue
			CORE_LOCK_MUTEX(mRequestMutex)

				if (!mAcceptRequests || mShuttingDown)
					return 0;

			rid = ++mRequestCount;
			req = CORE_NEW Request(channel, requestType, rData, retryCount, rid);

			if (!forceSynchronous)
			{
				mRequestQueue.push_front(req);
				notifyWorkers();
				return rid;
			}
		}


		processRequestResponse(req, true);

		return rid;

	}

	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::addRequestWithRID(WorkQueue::RequestID rid, uint16 channel, 
		uint16 requestType, const Any& rData, uint8 retryCount)
	{
		// lock to push request to the queue
		CORE_LOCK_MUTEX(mRequestMutex)

		if (mShuttingDown)
			return;

		Request* req = CORE_NEW Request(channel, requestType, rData, retryCount, rid);

		mRequestQueue.push_back(req);
		notifyWorkers();
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::abortRequest(RequestID id)
	{
		CORE_LOCK_MUTEX(mProcessMutex)

		// NOTE: Pending requests are exist any of RequestQueue, ProcessQueue and
		// ResponseQueue when keeping ProcessMutex, so we check all of these queues.

		for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
		{
			if ((*i)->getID() == id)
			{
				(*i)->abortRequest();
				break;
			}
		}

		{
			CORE_LOCK_MUTEX(mRequestMutex)

			for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
			{
				if ((*i)->getID() == id)
				{
					(*i)->abortRequest();
					break;
				}
			}
		}

		{
			CORE_LOCK_MUTEX(mResponseMutex)

			for (ResponseQueue::iterator i = mResponseQueue.begin(); i != mResponseQueue.end(); ++i)
			{
				if( (*i)->getRequest()->getID() == id )
				{
					(*i)->abortRequest();
					break;
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::abortRequestsByChannel(uint16 channel)
	{
		CORE_LOCK_MUTEX(mProcessMutex)

		for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
		{
			if ((*i)->getChannel() == channel)
			{
				(*i)->abortRequest();
			}
		}

		{
			CORE_LOCK_MUTEX(mRequestMutex)

			for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
			{
				if ((*i)->getChannel() == channel)
				{
					(*i)->abortRequest();
				}
			}
		}

		{
			CORE_LOCK_MUTEX(mResponseMutex)

			for (ResponseQueue::iterator i = mResponseQueue.begin(); i != mResponseQueue.end(); ++i)
			{
				if( (*i)->getRequest()->getChannel() == channel )
				{
					(*i)->abortRequest();
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::abortAllRequests()
	{
		CORE_LOCK_MUTEX(mProcessMutex)

		for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
		{
			(*i)->abortRequest();
		}

		{
			CORE_LOCK_MUTEX(mRequestMutex)

			for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
			{
				(*i)->abortRequest();
			}
		}

		{
			CORE_LOCK_MUTEX(mResponseMutex)

			for (ResponseQueue::iterator i = mResponseQueue.begin(); i != mResponseQueue.end(); ++i)
			{
				(*i)->abortRequest();
			}
		}
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::setPaused(bool pause)
	{
		CORE_LOCK_MUTEX(mRequestMutex)

		mPaused = pause;
	}
	//---------------------------------------------------------------------
	bool DefaultWorkQueueBase::isPaused() const
	{
		return mPaused;
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::setRequestsAccepted(bool accept)
	{
		CORE_LOCK_MUTEX(mRequestMutex)

		mAcceptRequests = accept;
	}
	//---------------------------------------------------------------------
	bool DefaultWorkQueueBase::getRequestsAccepted() const
	{
		return mAcceptRequests;
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::_processNextRequest()
	{
		Request* request = 0;
		{
			// scoped to only lock while retrieving the next request
			CORE_LOCK_MUTEX(mProcessMutex)
			{
				CORE_LOCK_MUTEX(mRequestMutex)

				if (!mRequestQueue.empty())
				{
					request = mRequestQueue.front();
					mRequestQueue.pop_front();
					//request = mRequestQueue.back();
					//mRequestQueue.pop_back();
					mProcessQueue.push_back( request );
				}
			}
		}

		if (request)
		{
			processRequestResponse(request, false);
		}


	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::processRequestResponse(Request* r, bool synchronous)
	{
		Response* response = processRequest(r);

		CORE_LOCK_MUTEX(mProcessMutex)

		RequestQueue::iterator it;
		for( it = mProcessQueue.begin(); it != mProcessQueue.end(); ++it )
		{
			if( (*it) == r )
			{
				mProcessQueue.erase( it );
				break;
			}
		}

		if (response)
		{
			if (!response->succeeded())
			{
				// Failed, should we retry?
				const Request* req = response->getRequest();
				if (req->getRetryCount())
				{
					addRequestWithRID(req->getID(), req->getChannel(), req->getType(), req->getData(), 
						req->getRetryCount() - 1);
					// discard response (this also deletes request)
					CORE_DELETE response;
					return;
				}
			}
			if (synchronous)
			{
				processResponse(response);
				CORE_DELETE response;
			}
			else
			{
				if( response->getRequest()->getAborted() )
				{
					// destroy response user data
					response->abortRequest();
				}
				// Queue response
				CORE_LOCK_MUTEX(mResponseMutex)
				mResponseQueue.push_back(response);
				// no need to wake thread, this is processed by the main thread
			}

		}
		else
		{
			// no response, delete request
			CORE_DELETE r;
		}

	}

	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::processResponses() 
	{
		unsigned long msStart = mTimer.getMilliseconds();
		unsigned long msCurrent = 0;

		// keep going until we run out of responses or out of time
		while(true)
		{
			Response* response = 0;
			{
				CORE_LOCK_MUTEX(mResponseMutex)

				if (mResponseQueue.empty())
					break; // exit loop
				else
				{
					response = mResponseQueue.front();
					mResponseQueue.pop_front();
				}
			}

			if (response)
			{
				processResponse(response);

				CORE_DELETE response;

			}

			// time limit
			if (mResposeTimeLimitMS)
			{
				msCurrent = mTimer.getMilliseconds();
				if (msCurrent - msStart > mResposeTimeLimitMS)
					break;
			}
		}
	}
	//---------------------------------------------------------------------
	WorkQueue::Response* DefaultWorkQueueBase::processRequest(Request* r)
	{
		RequestHandlerListByChannel handlerListCopy;
		{
			// lock the list only to make a copy of it, to maximise parallelism
			CORE_LOCK_RW_MUTEX_READ(mRequestHandlerMutex);
			
			handlerListCopy = mRequestHandlers;
			
		}

		Response* response = 0;

		RequestHandlerListByChannel::iterator i = handlerListCopy.find(r->getChannel());
		if (i != handlerListCopy.end())
		{
			RequestHandlerList& handlers = i->second;
			for (RequestHandlerList::reverse_iterator j = handlers.rbegin(); j != handlers.rend(); ++j)
			{
				// threadsafe call which tests canHandleRequest and calls it if so 
				response = (*j)->handleRequest(r, this);

				if (response)
					break;
			}
		}

		return response;

	}
	//---------------------------------------------------------------------
	void DefaultWorkQueueBase::processResponse(Response* r)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(r->getRequest()->getChannel());
		if (i != mResponseHandlers.end())
		{
			ResponseHandlerList& handlers = i->second;
			for (ResponseHandlerList::reverse_iterator j = handlers.rbegin(); j != handlers.rend(); ++j)
			{
				if ((*j)->canHandleResponse(r, this))
				{
					(*j)->handleResponse(r, this);
				}
			}
		}
	}
	//---------------------------------------------------------------------

	void DefaultWorkQueueBase::WorkerFunc::operator()()
	{
		mQueue->_threadMain();
	}

	void DefaultWorkQueueBase::WorkerFunc::run()
	{
		mQueue->_threadMain();
	}
}
