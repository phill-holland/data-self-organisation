#include "threading/thread.h"
#include "fifo.h"
#include "schema.h"

#ifndef _ORGANISATION_GENERATOR
#define _ORGANISATION_GENERATOR

namespace organisation
{
    class population;

    class generator : public core::threading::thread
    {
        population *pop;

    public:
	    void background(core::threading::thread *bt);

    public:
        generator(population *source) { pop = source; }

    protected:
        organisation::schema get();
    };
};

#endif