#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include "fifo.h"
#include "schemas.h"
#include "parallel/program.hpp"
#include "parallel/front.hpp"
#include <random>
#include <atomic>

#ifndef _ORGANISATION_POPULATION
#define _ORGANISATION_POPULATION

namespace organisation
{
    namespace populations
    {        
        class parameters
        {
        public:
            ::parallel::device *dev;
            ::parallel::queue *q;
            
            parallel::parameters params;
        
            organisation::data mappings;
            std::vector<std::string> expected;

            int clients;
            int fronts;
            int size;
        };

        class region
        {
        public:
            int start, end;

        public:
            region(int _start = 0, int _end = 0)
            {
                start = _start;
                end = _end;
            }
        };

        class results
        {
        public:
            float average;
            float best;
            int index;

        public:
            results() { average = 0.0f; best = 0.0f; index = 0; }
        };

        class population
        {
            friend class generator;
            friend class collector;

            static const int minimum = 100, maximum = 10000;
            //static const int fronts = 200;

            static std::mt19937_64 generator;

            //dominance::kdtree::kdtree *approximationA, *approximationB;
            organisation::parallel::front *frontA, *frontB;

            organisation::schemas *schemas;            
            organisation::schema **intermediateA, **intermediateB, **intermediateC;

            parallel::program *programs;

            int dimensions;

            parameters settings;

            bool init;

        public:
            population(parameters &params) { makeNull(); reset(params); }

            ~population() { cleanup(); }

            bool initalised() { return init; }
            void reset(parameters &params);

            void clear();
                    
            organisation::schema go(std::vector<std::string> expected, int &count, int iterations = 0);

        void generate();

        protected:
            bool get(schema &destination, region r, organisation::parallel::front *front);
            bool set(schema &source, region r, organisation::parallel::front *front);
            
        protected:
            schema *best(region r, organisation::parallel::front *front);
            schema *worst(region r, organisation::parallel::front *front);

        protected:
            void pull(organisation::schema **buffer, region r, organisation::parallel::front *front);
            void push(organisation::schema **buffer, region r, organisation::parallel::front *front);

        protected:
            void pick(region r, organisation::parallel::front *destination);

        protected:
            results execute(organisation::schema **buffer, std::vector<std::string> expected);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif