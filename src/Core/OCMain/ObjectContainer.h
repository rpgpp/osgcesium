#ifndef _OC_OBJECTCONTAINER_H__
#define _OC_OBJECTCONTAINER_H__

#include "OCMain/ThreadDefine.h"
#include "OCMain/osg.h"
#include "OCMain/IteratorWrapper.h"
#include "OCMain/stl.h"

namespace OC
{
	template<class T>
	class ObserverObjectCacher
	{
	public:
		ObserverObjectCacher(){}
		~ObserverObjectCacher(){}
		typedef std::map<String,osg::observer_ptr<T> > ObserverCacheMap;
		void insert(T* obj)
		{
			String name = obj->getName();
			if (name.empty())
			{
				return;
			}
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			mBatchObjects[name] = obj;
		}

		void clear()
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			mBatchObjects.clear();
		}

		MapIterator<ObserverCacheMap> getIterator()
		{
			return MapIterator<ObserverCacheMap>(mBatchObjects);
		}

		T* find(String name)
		{
			bool needClear = false;

			osg::ref_ptr<T> obj;

			{
				CORE_LOCK_RW_MUTEX_READ(BatchLock)
				typename ObserverCacheMap::const_iterator it = mBatchObjects.find(name);
				if (it != mBatchObjects.end())
				{
					needClear = !it->second.lock(obj);
				}
			}

			if (needClear)
			{
				clearInvalid();
			}

			return obj.release();
		}

		void remove(String name)
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			typename ObserverCacheMap::const_iterator it = mBatchObjects.find(name);
			if (it != mBatchObjects.end())
			{
				mBatchObjects.erase(it);
			}
		}

		long size()
		{
			return (long)mBatchObjects.size();
		}


		void clearInvalid()
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			typename ObserverCacheMap::iterator it = mBatchObjects.begin();
			while (it != mBatchObjects.end())
			{
				if (!it->second.valid())
				{
#if WIN32
					it = mBatchObjects.erase(it);
#else
					mBatchObjects.erase(it);
					it++;
#endif
				}
				else
				{
					it++;
				}
			}
		}

		CORE_RW_MUTEX(BatchLock)
	private:
		ObserverCacheMap mBatchObjects;
	};

	template<class T>
	class RefObjectCacher
	{
	public:
		RefObjectCacher(){}
		RefObjectCacher(const RefObjectCacher& rfr)
		{
			mBatchObjects = rfr.mBatchObjects;
		}
		~RefObjectCacher(){}

		inline RefObjectCacher& operator = (const RefObjectCacher& rfr)
		{
			mBatchObjects = rfr.mBatchObjects;
			return *this;
		}

		typedef std::map<String,osg::ref_ptr<T> > RefCacheMap;
		void insert(T* obj)
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			String name = obj->getName();
			if (name.empty())
			{
				return;
			}
			if (mBatchObjects.find(name) == mBatchObjects.end())
			{
				mBatchObjects[name] = obj;
			}
		}

		MapIterator<RefCacheMap> getIterator()
		{
			return MapIterator<RefCacheMap>(mBatchObjects);
		}

		void clear()
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			mBatchObjects.clear();
		}

		T* find(String name)
		{
			CORE_LOCK_RW_MUTEX_READ(BatchLock)
			typename RefCacheMap::const_iterator it = mBatchObjects.find(name);
			if (it != mBatchObjects.end())
			{
				return it->second.get();
			}
			return NULL;
		}

		void remove(String name)
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			typename RefCacheMap::iterator it = mBatchObjects.find(name);
			if (it != mBatchObjects.end())
			{
				mBatchObjects.erase(it);
			}
		}

		long size()
		{
			return (long)mBatchObjects.size();
		}
		CORE_RW_MUTEX(BatchLock)
	private:
		RefCacheMap mBatchObjects;
	};

	class StringIntMap
	{
	public:
		StringIntMap(){}
		~StringIntMap(){}

		int find(String shareName)
		{
			CORE_LOCK_RW_MUTEX_READ(BatchLock)
			std::map<String,int>::iterator it = mRecords.find(shareName);
			return it == mRecords.end() ? -1 : it->second;
		}

		void insert(String shareName,int shareID)
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			mRecords[shareName] = shareID;
		}
		CORE_RW_MUTEX(BatchLock)
	private:
		std::map<String,int> mRecords;
	};

	class StringLongMap
	{
	public:
		StringLongMap(){}
		~StringLongMap(){}

		long find(String shareName)
		{
			CORE_LOCK_RW_MUTEX_READ(BatchLock)
			std::map<String,long>::iterator it = mRecords.find(shareName);
			return it == mRecords.end() ? -1 : it->second;
		}

		void insert(String shareName,long shareID)
		{
			CORE_LOCK_RW_MUTEX_WRITE(BatchLock)
			mRecords[shareName] = shareID;
		}
		CORE_RW_MUTEX(BatchLock)

		void clear()
		{
			mRecords.clear();
		}
	private:
		std::map<String,long> mRecords;
	};

}

#endif