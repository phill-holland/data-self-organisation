#include "schema.h"
#include "data.h"
#include <random>
#include <atomic>

#ifndef _ORGANISATION_SCHEMAS
#define _ORGANISATION_SCHEMAS

namespace organisation
{
    class schemas
    {
    public:
        schema **data;

    private:
        std::atomic<int> *locks;

        int length;

        bool init;
        
    public:
        schemas(int width, int height, int depth, int size) { makeNull(); reset(width, height, depth, size); }
        ~schemas() { cleanup(); }

        bool initalised() { return init; }
        void reset(int width, int height, int depth, int size);

        bool clear();

        bool get(schema &destination, int index);
        bool set(schema &source, int index);

        schema *lock(int index);
        bool unlock(int index);

        bool generate(organisation::data &source);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif