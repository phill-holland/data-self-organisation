//#include "threading/thread.h"
//#include "threading/semaphore.h"
#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include "fifo.h"
#include "schemas.h"
#include "parallel/program.hpp"
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

        class population// : public core::threading::thread
        {
            friend class generator;
            friend class collector;

            static const int minimum = 100, maximum = 10000;
            //static const int threads = 6;

            static std::mt19937_64 generator;

            //const static int queues = 200;

            //core::queue::fifo<schema, queues> outgoing;
            //core::queue::fifo<schema, queues> incoming;

            dominance::kdtree::kdtree *approximation;

            //schema **data, **intermediate;
            //schema **left, **right;
            //std::atomic<int> *locks;

            organisation::schemas *schemas;
            
            organisation::schema **intermediateA, **intermediateB, **intermediateC;

            parallel::program *programs;

            
            //int size;

            int dimensions;

            parameters settings;

            //organisation::data mappings;
            //std::vector<int> lengths;

            //organisation::generator generating;
            //organisation::collector collecting;

            //core::threading::semaphore::token token;

            bool init;

        //public:
            //void background(core::threading::thread *bt);

        public:
            population(parameters &params)//organisation::data source, std::vector<std::string> expected, int size) 
            //: generating(this), collecting(this)
                { makeNull(); reset(params); }//source, expected, size); }

            ~population() { cleanup(); }

            bool initalised() { return init; }
            void reset(parameters &params);//organisation::data source, std::vector<std::string> expected, int size);

            void clear();
                    
            organisation::schema go(std::vector<std::string> expected, int &count, int iterations = 0);

            //schema top();

            //void start() 
            //{ 
                //collecting.start();
                //generating.start(); 
            //}

        void generate();

        protected:
            bool get(schema &destination, region r);
            bool set(schema &source, region r);
            
        protected:
            //std::tuple<std::vector<std::string>,float> run(std::vector<std::string> expected, const float mutation);
            //void back(schema **destination, schema **source, int thread);

            schema *best(region r);//schema &destination, schema &competition);
            //int worst(schema **source, int start, int end);
            schema *worst(region r);

            void pull(organisation::schema **buffer, region r);
            void push(organisation::schema **buffer, region r);

        protected:
            results execute(organisation::schema **buffer, std::string expected);

        //protected:
            //int pull(schema &destination);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif