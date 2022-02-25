#ifndef _OC_THREADDEFINE_H__
#define _OC_THREADDEFINE_H__

#include "BaseDefine.h"

#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <OpenThreads/ReadWriteMutex>
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>


#define CORE_AUTO_MUTEX_NAME mutex
#define CORE_AUTO_MUTEX mutable OpenThreads::ReentrantMutex CORE_AUTO_MUTEX_NAME;
#define CORE_LOCK_AUTO_MUTEX OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> coreAutoMutexLock(CORE_AUTO_MUTEX_NAME);

// like CORE_AUTO_MUTEX but mutex held by pointer
#define CORE_AUTO_SHARED_MUTEX mutable OpenThreads::ReentrantMutex *CORE_AUTO_MUTEX_NAME;
#define CORE_LOCK_AUTO_SHARED_MUTEX assert(CORE_AUTO_MUTEX_NAME); OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> coreAutoMutexLock(*CORE_AUTO_MUTEX_NAME);
#define CORE_NEW_AUTO_SHARED_MUTEX assert(!CORE_AUTO_MUTEX_NAME); CORE_AUTO_MUTEX_NAME = new OpenThreads::ReentrantMutex();
#define CORE_DELETE_AUTO_SHARED_MUTEX assert(CORE_AUTO_MUTEX_NAME); delete CORE_AUTO_MUTEX_NAME;
#define CORE_COPY_AUTO_SHARED_MUTEX(from) assert(!CORE_AUTO_MUTEX_NAME); CORE_AUTO_MUTEX_NAME = from;
#define CORE_SET_AUTO_SHARED_MUTEX_NULL CORE_AUTO_MUTEX_NAME = 0;
#define CORE_MUTEX_CONDITIONAL(mutex) if (mutex)
#define CORE_THREAD_SYNCHRONISER(sync) OpenThreads::Condition sync;
#define CORE_THREAD_WAIT(sync, mutex, lock) sync.wait(&mutex);
#define CORE_THREAD_NOTIFY_ONE(sync) sync.signal(); 
#define CORE_THREAD_NOTIFY_ALL(sync) sync.broadcast(); 
// Read-write mutex
#define CORE_RW_MUTEX(name) mutable OpenThreads::ReadWriteMutex name;
#define CORE_LOCK_RW_MUTEX_READ(name) OpenThreads::ScopedReadLock corenameLock(name);
#define CORE_LOCK_RW_MUTEX_WRITE(name) OpenThreads::ScopedWriteLock corenameLock(name);
// Thread objects and related functions
#define CORE_THREAD_TYPE OpenThreads::Thread

#define CORE_THREAD_SLEEP(ms) OpenThreads::Thread::microSleep(ms);

#define CORE_MUTEX(name) OpenThreads::ReentrantMutex name;
#define CORE_STATIC_MUTEX(name) static OpenThreads::ReentrantMutex name;
#define CORE_STATIC_MUTEX_INSTANCE(name) OpenThreads::ReentrantMutex name;
#define CORE_LOCK_MUTEX(name) OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> exclusive(name);
#define CORE_LOCK_MUTEX_NAMED(mutexName, lockName) OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lockName(mutexName);

#define  CORE_THREAD_SUPPORT 1

#endif