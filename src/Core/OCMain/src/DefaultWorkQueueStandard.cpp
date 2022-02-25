#include "DefaultWorkQueueStandard.h"

namespace OC
{
	//---------------------------------------------------------------------
	DefaultWorkQueue::DefaultWorkQueue(const String& name)
	: DefaultWorkQueueBase(name)
	{
	}
	//---------------------------------------------------------------------
	DefaultWorkQueue::~DefaultWorkQueue()
	{
		shutdown();
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueue::startup(bool forceRestart)
	{
		if (mIsRunning)
		{
			if (forceRestart)
				shutdown();
			else
				return;
		}

		mShuttingDown = false;

		//mWorkerFunc = EC_NEW_T(WorkerFunc(this), MEMCATEGORY_GENERAL);

		
		mNumThreadsRegisteredWithRS = 0;
		for (uint8 i = 0; i < mWorkerThreadCount; ++i)
		{
			//EC_THREAD_CREATE(t, *mWorkerFunc);
			WorkQueueThread* t = new WorkQueueThread(this);
			t->start();
			mWorkers.push_back(t);
		}

		

		mIsRunning = true;
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueue::notifyThreadRegistered()
	{
		CORE_LOCK_MUTEX(mInitMutex)

		++mNumThreadsRegisteredWithRS;

		// wake up main thread
		CORE_THREAD_NOTIFY_ALL(mInitSync);

	}
	//---------------------------------------------------------------------
	void DefaultWorkQueue::shutdown()
	{
		if( !mIsRunning )
			return;

		mShuttingDown = true;
		abortAllRequests();

		// wake all threads (they should check shutting down as first thing after wait)
		CORE_THREAD_NOTIFY_ALL(mRequestCondition)

		// all our threads should have been woken now, so join
		for (WorkerThreadList::iterator i = mWorkers.begin(); i != mWorkers.end(); ++i)
		{
			(*i)->join();
			delete (*i);
		}
		mWorkers.clear();

		if (mWorkerFunc)
		{
			CORE_SAFE_DELETE(mWorkerFunc);
		}


		mIsRunning = false;


	}
	//---------------------------------------------------------------------
	void DefaultWorkQueue::notifyWorkers()
	{
		// wake up waiting thread
		CORE_THREAD_NOTIFY_ONE(mRequestCondition)
	}

	//---------------------------------------------------------------------
	void DefaultWorkQueue::waitForNextRequest()
	{
		// Lock; note that EC_THREAD_WAIT will free the lock
		CORE_LOCK_MUTEX_NAMED(mRequestMutex, queueLock);
		if (mRequestQueue.empty())
		{
			// frees lock and suspends the thread
			CORE_THREAD_WAIT(mRequestCondition, mRequestMutex, queueLock);
		}
		// When we get back here, it's because we've been notified 
		// and thus the thread has been woken up. Lock has also been
		// re-acquired, but we won't use it. It's safe to try processing and fail
		// if another thread has got in first and grabbed the request
	}
	//---------------------------------------------------------------------
	void DefaultWorkQueue::_threadMain()
	{
		// Spin forever until we're told to shut down
		while (!isShuttingDown())
		{
			waitForNextRequest();
			_processNextRequest();
		}


	}

}
