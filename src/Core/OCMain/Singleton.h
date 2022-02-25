#ifndef _OC_SINGLETON_H__
#define _OC_SINGLETON_H__

#pragma warning (disable : 4311)
#pragma warning (disable : 4312)
#pragma warning (disable : 4661)

#include "BaseDefine.h"

namespace OC {
    template <typename T> class CSingleton
    {
	private:
		/** \brief Explicit private copy constructor. This is a forbidden operation.*/
		CSingleton(const CSingleton<T> &);

		/** \brief Private operator= . This is a forbidden operation. */
		CSingleton& operator=(const CSingleton<T> &);
    
	protected:
#ifdef _WIN32
		static std::map<int,T*>	msPidton;
	public:
		CSingleton( void )
		{
			msPidton[CORE_CURRENT_PID] = static_cast< T* >( this );
		}
		~CSingleton( void )
		{}
		static T& getSingleton( void )
		{	return ( *msPidton[CORE_CURRENT_PID] ); }
		static T* getSingletonPtr( void )
		{	return msPidton[CORE_CURRENT_PID]; }
#else              
		static T* msSingleton;
	public:
		CSingleton( void )
		{
			msSingleton = static_cast< T* >( this );
		}
		~CSingleton( void )
		{}
		static T& getSingleton( void )
		{	return ( *msSingleton ); }
		static T* getSingletonPtr( void )
		{	return msSingleton; }
#endif

    };
	/** @} */
	/** @} */

}
#if defined ( CORE_GCC_VISIBILITY )
#   pragma GCC visibility pop
#endif

#endif
