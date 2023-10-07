#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "schema.h"

#ifndef _PARALLEL_FRONT
#define _PARALLEL_FRONT

namespace organisation
{    
    namespace parallel
    {
        class front
        {
            ::parallel::device *dev;

            organisation::schema **schemas;

            float *deviceValues;
            int *deviceIsFront;
            int *deviceDominatedCount;
            
            int *hostIsFront;
            int *hostDominatedCount;
            float *hostValues;

            int terms;            
            int clients;

            bool init;
                    
        public:
            front(::parallel::device &dev, int terms, int clients) { makeNull(); reset(dev, terms, clients); }
            ~front() { cleanup(); }

            bool initalised() { return init; }
            void reset(::parallel::device &dev, int terms, int clients);

            void set(organisation::schema *source, int client);
            void set(int *source, int client);
            
            organisation::schema *get(int client);
            
            bool is_front(int client);
            int rank(int client);

            int front_count();
            int rank_front_count();
            
            void run(::parallel::queue *q);
            void run2(::parallel::queue *q);
            void run3(::parallel::queue *q);
            void run4(::parallel::queue *q);

            void extra(::parallel::queue *q);

        public:
            void outputarb(int *source, int length);
            void outputarb(sycl::float4 *source, int length);
            void outputarb(float *source, int length);

        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif