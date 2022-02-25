#ifndef __Common_H__
#define __Common_H__

#include "BaseDefine.h"

#if defined ( CORE_GCC_VISIBILITY )
#   pragma GCC visibility push(default)
#endif

#if defined ( CORE_GCC_VISIBILITY )
#   pragma GCC visibility pop
#endif

namespace OC {
	/// Name / value parameter pair (first = name, second = value)
	typedef std::map<String, String> NameValuePairList;

	/// Utility class to generate a sequentially numbered series of names
	class _MainExport NameGenerator
	{
	protected:
		String mPrefix;
		unsigned long long int mNext;
		CORE_AUTO_MUTEX
	public:
		NameGenerator(const NameGenerator& rhs)
			: mPrefix(rhs.mPrefix), mNext(rhs.mNext) {}
		
		NameGenerator(const String& prefix) : mPrefix(prefix), mNext(1) {}

		/// Generate a new name
		String generate()
		{
			CORE_LOCK_AUTO_MUTEX
			std::ostringstream s;
			s << mPrefix << mNext++;
			return s.str();
		}

		/// Reset the internal counter
		void reset()
		{
			CORE_LOCK_AUTO_MUTEX
			mNext = 1ULL;
		}

		/// Manually set the internal counter (use caution)
		void setNext(unsigned long long int val)
		{
			CORE_LOCK_AUTO_MUTEX
			mNext = val;
		}

		/// Get the internal counter
		unsigned long long int getNext() const
		{
			// lock even on get because 64-bit may not be atomic read
			CORE_LOCK_AUTO_MUTEX
			return mNext;
		}
	};

	/** Template class describing a simple pool of items.
	*/
	template <typename T>
	class Pool
	{
	protected:
		typedef typename std::list<T> ItemList;
		ItemList mItems;
		CORE_AUTO_MUTEX
	public:
		Pool() {} 
		virtual ~Pool() {}

		/** Get the next item from the pool.
		@return pair indicating whether there was a free item, and the item if so
		*/
		virtual std::pair<bool, T> removeItem()
		{
			CORE_LOCK_AUTO_MUTEX
			std::pair<bool, T> ret;
			if (mItems.empty())
			{
				ret.first = false;
			}
			else
			{
				ret.first = true;
				ret.second = mItems.front();
				mItems.pop_front();
			}
			return ret;
		}

		/** Add a new item to the pool. 
		*/
		virtual void addItem(const T& i)
		{
			CORE_LOCK_AUTO_MUTEX
			mItems.push_front(i);
		}
		/// Clear the pool
		virtual void clear()
		{
			CORE_LOCK_AUTO_MUTEX
			mItems.clear();
		}
	};
}

#endif
