#pragma once

#if defined(__APPLE__) && defined(__MACH__)
#include <libkern/OSAtomic.h>
#endif

namespace O2G2
{
    template<typename T> class TO2G2ThreadSafeAddRefImpl : public T
    {
     public:
        TO2G2ThreadSafeAddRefImpl()
        {
            m_dwRef = 1;
        }
        virtual ~TO2G2ThreadSafeAddRefImpl(){};

        long addRef()
        {
            return ++m_dwRef;
        }
        
        long release()
        {
            long lResult = --m_dwRef;
            if (lResult == 0)
                delete this;
            return lResult;
        }
        
     private:
        volatile std::atomic_long m_dwRef{ 0 };
    };
}
