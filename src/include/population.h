#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include "semaphore.h"
#include <random>

#ifndef _ORGANISATION_POPULATION
#define _ORGANISATION_POPULATION

namespace organisation
{
    class population
    {
        static const int minimum = 100, maximum = 10000;
        static const int threads = 12;

        static std::mt19937_64 generator;

        dominance::kdtree::kdtree *approximation;

        schema **data, **intermediate;
        int size;

        int dimensions;

        std::vector<int> lengths;

        bool init;

    public:
        population(std::vector<std::string> expected, int size) { makeNull(); reset(expected, size); }
        ~population() { cleanup(); }

        bool initalised() { return init; }
        void reset(std::vector<std::string> expected, int size);

        void clear();
        
        void generate(organisation::data &source);
        organisation::schema go(organisation::data &source, std::vector<std::string> expected, int &count, int iterations = 0);

        schema top();
                
    protected:
        std::tuple<std::vector<std::string>,schema*> run(schema *destination, organisation::data *source, std::vector<std::string> expected, const float mutation);
        schema *best();
        int worst();

    protected:
        bool set(int index, schema &source);
        
    protected:
        void makeNull();
        void cleanup();
    };
};

#endif