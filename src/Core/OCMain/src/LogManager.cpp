#include "LogManager.h"
#include "Exception.h"

namespace OC {

    //-----------------------------------------------------------------------
	//LogManager* msSingleton = NULL;
#if SingletonConfig == SingletonNormal
	template<>LogManager* CSingleton<LogManager>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int, LogManager*> CSingleton<LogManager>::msPidton;
#endif

    //-----------------------------------------------------------------------
    LogManager::LogManager()
    {
        mDefaultLog = NULL;
		//msSingleton = this;
    }
    //-----------------------------------------------------------------------
    LogManager::~LogManager()
    {
		CORE_LOCK_AUTO_MUTEX
        // Destroy all logs
        LogList::iterator i;
        for (i = mLogs.begin(); i != mLogs.end(); ++i)
        {
            CORE_DELETE i->second;
        }
    }
    //-----------------------------------------------------------------------
    Log* LogManager::createLog( const String& name, bool defaultLog, bool debuggerOutput, 
		bool suppressFileOutput)
    {
		CORE_LOCK_AUTO_MUTEX

        Log* newLog = CORE_NEW Log(name, debuggerOutput, suppressFileOutput);

        if( !mDefaultLog || defaultLog )
        {
            mDefaultLog = newLog;
        }

        mLogs.insert( LogList::value_type( name, newLog ) );

        return newLog;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::getDefaultLog()
    {
		CORE_LOCK_AUTO_MUTEX
        return mDefaultLog;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::setDefaultLog(Log* newLog)
    {
		CORE_LOCK_AUTO_MUTEX
        Log* oldLog = mDefaultLog;
        mDefaultLog = newLog;
        return oldLog;
    }
    //-----------------------------------------------------------------------
    Log* LogManager::getLog( const String& name)
    {
		CORE_LOCK_AUTO_MUTEX
        LogList::iterator i = mLogs.find(name);
        if (i != mLogs.end())
            return i->second;
        else
            CORE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Log not found. ", "LogManager::getLog");


    }
	//-----------------------------------------------------------------------
	void LogManager::destroyLog(const String& name)
	{
		LogList::iterator i = mLogs.find(name);
		if (i != mLogs.end())
		{
			if (mDefaultLog == i->second)
			{
				mDefaultLog = 0;
			}
			CORE_DELETE i->second;
			mLogs.erase(i);
		}

		// Set another default log if this one removed
		if (!mDefaultLog && !mLogs.empty())
		{
			mDefaultLog = mLogs.begin()->second;
		}
	}
	//-----------------------------------------------------------------------
	void LogManager::destroyLog(Log* log)
	{
		destroyLog(log->getName());
	}
    //-----------------------------------------------------------------------
    void LogManager::logMessage( const String& message, LogMessageLevel lml, bool maskDebug)
    {
		CORE_LOCK_AUTO_MUTEX
		if (mDefaultLog)
		{
			mDefaultLog->logMessage(message, lml, maskDebug);
		}
    }
    //-----------------------------------------------------------------------
    void LogManager::setLogDetail(LoggingLevel ll)
    {
		CORE_LOCK_AUTO_MUTEX
		if (mDefaultLog)
		{
	        mDefaultLog->setLogDetail(ll);
		}
    }
	//---------------------------------------------------------------------
	Log::Stream LogManager::stream(LogMessageLevel lml, bool maskDebug)
	{
		CORE_LOCK_AUTO_MUTEX
		if (mDefaultLog)
			return mDefaultLog->stream(lml, maskDebug);
		else
			CORE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Default log not found. ", "LogManager::stream");

	}
}
