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
        schema **data;
        std::atomic<int> *locks;

        int length;

        bool init;
        
    public:
        schemas(int size) { makeNull(); reset(size); }
        ~schemas() { cleanup(); }

        bool initalised() { return init; }
        void reset(int size);

        bool clear();

        bool get(schema &destination, int index);
        bool set(schema &source, int index);

        bool generate(organisation::data &source);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif