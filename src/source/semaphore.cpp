#include "semaphore.h"

threading::semaphore::token::token() 
{ 
    lock.store(0, std::memory_order_release); 
}

bool threading::semaphore::token::obtain()
{
    int expected = 0;
    return lock.compare_exchange_weak(expected, 1, std::memory_order_release, std::memory_order_relaxed);
}

bool threading::semaphore::token::release()
{
    int expected = 1;
    return lock.compare_exchange_weak(expected, 0, std::memory_order_release, std::memory_order_relaxed);
}