#ifndef __SharedPtr_H__
#define __SharedPtr_H__

#include "AtomicScalar.h"

namespace OC {
 /** \addtogroup Core
    *  @{
    */
    /** \addtogroup General
    *  @{
    */

    /// The method to use to free memory on destruction
    enum SharedPtrFreeMethod
    {
        /// Use CORE_DELETE to free the memory
        SPFM_DELETE,
        /// Use CORE_DELETE_T to free (only MEMCATEGORY_GENERAL supported)
        SPFM_DELETE_T,
        /// Use CORE_FREE to free (only MEMCATEGORY_GENERAL supported)
        SPFM_FREE,
        /// Don`t free resource at all, lifetime controlled externally.
        SPFM_NONE
    };

    struct SharedPtrInfo {
        inline SharedPtrInfo() 
            : useCount(1)
        {}

        virtual ~SharedPtrInfo() {}

        AtomicScalar<unsigned>  useCount;
    };

    struct SharedPtrInfoNone : public SharedPtrInfo
    {
    };

    template <class T>
    class SharedPtrInfoDelete : public SharedPtrInfo
    {
        T* mObject;
    public:
        inline SharedPtrInfoDelete(T* o) : mObject(o) {}

        virtual ~SharedPtrInfoDelete()
        {
            CORE_SAFE_DELETE(mObject);
        }
    };

    template <class T>
    class SharedPtrInfoDeleteT : public SharedPtrInfo
    {
        T* mObject;
    public:
        inline SharedPtrInfoDeleteT(T* o) : mObject(o) {}

        virtual ~SharedPtrInfoDeleteT()
        {
            CORE_SAFE_DELETE(mObject);
        }
    };

    template <class T>
    class SharedPtrInfoFree : public SharedPtrInfo
    {
        T* mObject;
    public:
        inline SharedPtrInfoFree(T* o) : mObject(o) {}        

        virtual ~SharedPtrInfoFree()
        {
            CORE_SAFE_DELETE(mObject);
        }
    };

    template<class T, class U>
    inline SharedPtr<T> static_pointer_cast(const SharedPtr<U>& r)
    {
        if(r.pRep) {
            ++r.pInfo->useCount;
            return SharedPtr<T>(static_cast<T*>(r.pRep), r.pInfo);
        }
        return SharedPtr<T>();
    }

    template<class T, class U>
    inline SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U>& r)
    {
        T* rep = dynamic_cast<T*>(r.pRep);
        if(rep) {
            ++r.pInfo->useCount;
            return SharedPtr<T>(rep, r.pInfo);
        }
        return SharedPtr<T>();
    }

    /** Reference-counted shared pointer, used for objects where implicit destruction is 
        required. 
    @remarks
        This is a standard shared pointer implementation which uses a reference 
        count to work out when to delete the object. 
    @par
        If CORE_THREAD_SUPPORT is defined to be 1, use of this class is thread-safe.
    */
    template<class T> class SharedPtr
    {
        template<typename Y> friend class SharedPtr;
        template<typename Y, typename X> friend SharedPtr<Y> static_pointer_cast(const SharedPtr<X>& r);
        template<typename Y, typename X> friend SharedPtr<Y> dynamic_pointer_cast(const SharedPtr<X>& r);
    protected:
        /* DO NOT ADD MEMBERS TO THIS CLASS!
         *
         * The average CORE application has *thousands* of them. Every extra
         * member causes extra memory use in general, and causes extra padding
         * to be added to a multitude of structures. 
         *
         * Everything you need to do can be acomplished by creatively working 
         * with the SharedPtrInfo object.
         *
         * There is no reason for this object to ever have more than two members.
         */

        T*             pRep;
        SharedPtrInfo* pInfo;

        SharedPtr(T* rep, SharedPtrInfo* info) : pRep(rep), pInfo(info)
        {
        }

    public:
        /** Constructor, does not initialise the SharedPtr.
            @remarks
                <b>Dangerous!</b> You have to call bind() before using the SharedPtr.
        */
        SharedPtr() : pRep(0), pInfo(0)
        {}

    private:
        static SharedPtrInfo* createInfoForMethod(T* rep, SharedPtrFreeMethod method)
        {
            switch(method) {
                case SPFM_DELETE:   return CORE_NEW SharedPtrInfoDelete<T>(rep);
                case SPFM_DELETE_T: return CORE_NEW SharedPtrInfoDeleteT<T>(rep);
                case SPFM_FREE:     return CORE_NEW SharedPtrInfoFree<T>(rep);
                case SPFM_NONE:     return CORE_NEW SharedPtrInfoNone();
            }
            assert(!"Bad method");
            return 0;
        }
    public:

        /** Constructor.
        @param rep The pointer to take ownership of
        @param inFreeMethod The mechanism to use to free the pointer
        */
        template< class Y>
        explicit SharedPtr(Y* rep, SharedPtrFreeMethod inFreeMethod = SPFM_DELETE) 
            : pRep(rep)
            , pInfo(rep ? createInfoForMethod(rep, inFreeMethod) : 0)
        {
        }

        SharedPtr(const SharedPtr& r)
            : pRep(r.pRep)
            , pInfo(r.pInfo)
        {
            if (pRep) 
            {
                ++pInfo->useCount;
            }
        }

        SharedPtr& operator=(const SharedPtr& r)
        {
            // One resource could have several non-controlling control blocks but only one controlling.
            assert(pRep != r.pRep || pInfo == r.pInfo || dynamic_cast<SharedPtrInfoNone*>(pInfo) || dynamic_cast<SharedPtrInfoNone*>(r.pInfo));
            if(pInfo == r.pInfo)
                return *this;

            // Swap current data into a local copy
            // this ensures we deal with rhs and this being dependent
            SharedPtr<T> tmp(r);
            swap(tmp);
            return *this;
        }
        
        /* For C++11 compilers, use enable_if to only expose functions when viable
         *
         * MSVC 2012 and earlier only claim conformance to C++98. This is fortunate,
         * because they don't support default template parameters
         */
#if __cplusplus >= 201103L && !defined( __APPLE__ )
        template<class Y,
            class = typename std::enable_if<std::is_convertible<Y*, T*>::value>::type>
#else
        template<class Y>
#endif
        SharedPtr(const SharedPtr<Y>& r)
            : pRep(r.pRep)
            , pInfo(r.pInfo)
        {
            if (pRep) 
            {
                ++pInfo->useCount;
            }
        }

        
#if __cplusplus >= 201103L && !defined( __APPLE__ )
        template<class Y,
                 class = typename std::enable_if<std::is_assignable<T*, Y*>::value>::type>
#else
        template<class Y>
#endif
        SharedPtr& operator=(const SharedPtr<Y>& r)
        {
            // One resource could have several non-controlling control blocks but only one controlling.
            assert(pRep != r.pRep || pInfo == r.pInfo || dynamic_cast<SharedPtrInfoNone*>(pInfo) || dynamic_cast<SharedPtrInfoNone*>(r.pInfo));
            if(pInfo == r.pInfo)
                return *this;
            
            // Swap current data into a local copy
            // this ensures we deal with rhs and this being dependent
            SharedPtr<T> tmp(r);
            swap(tmp);
            return *this;
        }

        ~SharedPtr() {
            release();
        }

        /// @deprecated use CORE::static_pointer_cast instead
        template<typename Y>
        SharedPtr<Y> staticCast() const
        {
            return static_pointer_cast<Y>(*this);
        }

        /// @deprecated use CORE::dynamic_pointer_cast instead
        template<typename Y>
        SharedPtr<Y> dynamicCast() const
        {
            return dynamic_pointer_cast<Y>(*this);
        }

        inline T& operator*() const { assert(pRep); return *pRep; }
        inline T* operator->() const { assert(pRep); return pRep; }
        inline T* get() const { return pRep; }

        /** Binds rep to the SharedPtr.
            @remarks
                Assumes that the SharedPtr is uninitialised!

            @warning
                The object must not be bound into a SharedPtr elsewhere

            @deprecated this api will be dropped. use reset(T*) instead
        */
        void bind(T* rep, SharedPtrFreeMethod inFreeMethod = SPFM_DELETE) {
            assert(!pRep && !pInfo);
            pInfo = createInfoForMethod(rep, inFreeMethod);
            pRep = rep;
        }

        inline bool unique() const { assert(pInfo && pInfo->useCount.get()); return pInfo->useCount.get() == 1; }

        /// @deprecated use use_count() instead
        unsigned int useCount() const { return use_count(); }

        long use_count() const { assert(pInfo && pInfo->useCount.get()); return pInfo->useCount.get(); }

        /// @deprecated this API will be dropped
        void setUseCount(unsigned value) { assert(pInfo); pInfo->useCount = value; }

        /// @deprecated use get() instead
        T* getPointer() const { return pRep; }

#if __cplusplus >= 201103L || CORE_COMPILER == CORE_COMPILER_MSVC && CORE_COMP_VER >= 1800
        explicit operator bool() const
        {
            return pRep != 0;
        }
#else
        static void unspecified_bool( SharedPtr*** )
        {
        }

        typedef void (*unspecified_bool_type)( SharedPtr*** );

        operator unspecified_bool_type() const
        {
            return pRep == 0 ? 0 : unspecified_bool;
        }
#endif

        /// @deprecated use SharedPtr::operator unspecified_bool_type() instead
        bool isNull(void) const { return pRep == 0; }

        /// @deprecated use reset() instead
        void setNull() { reset(); }

        void reset(void) {
            release();
        }

        void reset(T* rep) {
            SharedPtr(rep).swap(*this);
        }

    protected:

        inline void release(void)
        {
            if (pRep)
            {
                assert(pInfo);
                if(--pInfo->useCount == 0)
                    destroy();
            }

            pRep = 0;
            pInfo = 0;
        }

        /** IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS
         BEFORE SHUTTING CORE DOWN
         Use reset() before shutdown or make sure your pointer goes
         out of scope before CORE shuts down to avoid this. */
        inline void destroy(void)
        {
            assert(pRep && pInfo);
            CORE_SAFE_DELETE(pInfo);
        }

        inline void swap(SharedPtr<T> &other) 
        {
            std::swap(pRep, other.pRep);
            std::swap(pInfo, other.pInfo);
        }
    };

    template<class T, class U> inline bool operator==(SharedPtr<T> const& a, SharedPtr<U> const& b)
    {
        return a.get() == b.get();
    }

    template<class T, class U> inline bool operator!=(SharedPtr<T> const& a, SharedPtr<U> const& b)
    {
        return a.get() != b.get();
    }

    template<class T, class U> inline bool operator<(SharedPtr<T> const& a, SharedPtr<U> const& b)
    {
        return std::less<const void*>()(a.get(), b.get());
    }
    /** @} */
    /** @} */
}

#endif
