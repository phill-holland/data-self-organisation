#include "threading/thread.h"
#include "fifo.h"
#include "schema.h"
#include <chrono>

#ifndef _ORGANISATION_COLLECTOR
#define _ORGANISATION_COLLECTOR

namespace organisation
{
    class population;

    class collector : public core::threading::thread
    {
        std::chrono::high_resolution_clock::time_point previous;
        int counter;

        population *pop;
        schema temp;   

    public:
	    void background(core::threading::thread *bt);

    public:
        collector(population *source) 
        { 
            previous = std::chrono::high_resolution_clock::now();
            counter = 0;
            pop = source;             
        }
    };
};

#endif