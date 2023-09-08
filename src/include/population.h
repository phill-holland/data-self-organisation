#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include "thread.h"
#include "fifo.h"
#include "semaphore.h"
#include "generator.h"
#include "collector.h"
#include "schemas.h"
#include <random>
#include <atomic>

#ifndef _ORGANISATION_POPULATION
#define _ORGANISATION_POPULATION

namespace organisation
{
    class population// : public core::threading::thread
    {
        friend class generator;
        friend class collector;

        static const int minimum = 100, maximum = 10000;
        static const int threads = 12;

        static std::mt19937_64 generator;

        const static int queues = 200;

        core::queue::fifo<schema, queues> outgoing;
        core::queue::fifo<schema, queues> incoming;

        dominance::kdtree::kdtree *approximation;

        //schema **data, **intermediate;
        //schema **left, **right;
        //std::atomic<int> *locks;

        organisation::schemas *schemas;

        int size;

        int dimensions;

        organisation::data mappings;
        //std::vector<int> lengths;

        organisation::generator generating;
        organisation::collector collecting;

        threading::semaphore::token token;

        bool init;

    //public:
	    //void background(core::threading::thread *bt);

    public:
        population(organisation::data source, std::vector<std::string> expected, int size) 
        : generating(this), collecting(this)
            { makeNull(); reset(source, expected, size); }

        ~population() { cleanup(); }

        bool initalised() { return init; }
        void reset(organisation::data source, std::vector<std::string> expected, int size);

        void clear();
                
        organisation::schema go(std::vector<std::string> expected, int &count, int iterations = 0);

        schema top();

        void start() 
        { 
            //collecting.start();
            //generating.start(); 
        }

    void generate();

    protected:
        bool get(schema &destination);
        

    protected:
        std::tuple<std::vector<std::string>,float> run(std::vector<std::string> expected, const float mutation);
        //void back(schema **destination, schema **source, int thread);

        bool best(schema &destination);
        //int worst(schema **source, int start, int end);
        int worst(schema &destination);

    protected:
        int pull(schema &destination);

        bool set(schema &source);
        
    protected:
        void makeNull();
        void cleanup();
    };
};

#endif