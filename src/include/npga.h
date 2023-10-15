#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include "fifo.h"
#include "schemas.h"
#include "parallel/program.hpp"
#include "parallel/front.hpp"
#include "parameters.h"
#include "region.h"
#include "results.h"
#include <random>
#include <atomic>

#ifndef _ORGANISATION_NPGA
#define _ORGANISATION_NPGA

namespace organisation
{
    namespace populations
    {                
        class npga
        {
            static std::mt19937_64 generator;

            organisation::parallel::front *frontA, *frontB;

            std::vector<organisation::schema*> schemas;
            //std::vector<organisation::schema*> intermediateA, intermediateB, intermediateC;
            //organisation::schema **schemas;
            organisation::schema **intermediateA, **intermediateB, **intermediateC;

            float *distancesA, *distancesB;

            parallel::program *programs;

            int dimensions;

            parameters settings;

            bool init;

        public:
            npga(parameters &params) { makeNull(); reset(params); }
            ~npga() { cleanup(); }

            bool initalised() { return init; }
            void reset(parameters &params);

            void clear();
                    
            organisation::schema go(std::vector<std::string> expected, int &count, int iterations = 0);

            void generate();

        protected:
            bool get(schema &destination, organisation::parallel::front *front, float *distances);
            bool set(schema &source, organisation::parallel::front *front, float *distances);
            
        protected:
            schema *best(organisation::parallel::front *front, float *distances);
            schema *worst(organisation::parallel::front *front, float *distances);

        protected:
            void pull(organisation::schema **buffer, region r);
            void push(organisation::schema **buffer, region r);

        protected:
            void pick(region r, organisation::parallel::front *destination, float *distances);

        protected:
            void sort(region r, int dimension);   
            void crowded(region r, float *distances);

        protected:
            results execute(organisation::schema **buffer, std::vector<std::string> expected);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif