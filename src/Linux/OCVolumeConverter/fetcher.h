#ifndef _FETCHER_H__
#define _FETCHER_H__

#include "TaskWorkQueue.h"

using namespace OC;

class CFetcher : public osg::Referenced
{
public:
	CFetcher(String dir);
	~CFetcher();
	void start();
	void stop();
private:
	void clear();
	bool			mRunning;
	long			monitor_tick;
	float			mInterval;
	String			mMonitorDir;
	String			mMonitorDay;
	CORE_STATIC_MUTEX(mutex)
	static time_t	mStaticLastClearTick;
};



#endif // !_FETCHER_H__
