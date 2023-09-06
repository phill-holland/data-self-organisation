#include <atomic>

#ifndef _THREADING_SEMAPHORE
#define _THREADING_SEMAPHORE

namespace threading
{
    class semaphore
    {
    public:
        class token
        {
        public:
            std::atomic<int> lock;

        public:
            token();
            token(const token &source) { }

            bool obtain();
            bool release();		
        };

    protected:
        token *value;

    public:

        semaphore(token &source)
        {
            value = &source;
            while (!value->obtain()) {}
        }

        ~semaphore()
        {
            while (!value->release()) {}
        }
    };
};

#endif