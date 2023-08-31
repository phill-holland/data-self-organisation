#include "schema.h"
#include "kdtree.h"
#include "data.h"
#include <random>

#ifndef _ORGANISATION_POPULATION
#define _ORGANISATION_POPULATION

namespace organisation
{
    class population
    {
        static const int minimum = 100, maximum = 10000;
        static std::mt19937_64 generator;

        dominance::kdtree::kdtree *approximation;

        schema **data;
        int size;

        int dimensions;

        std::vector<int> lengths;

        bool init;

    public:
        population(std::vector<std::string> expected, int size) { makeNull(); reset(expected, size); }
        ~population() { cleanup(); }

        bool initalised() { return init; }
        void reset(std::vector<std::string> expected, int size);

        void generate(organisation::data &source);
        organisation::schema go(organisation::data &source, std::vector<std::string> expected, int iterations = 0);

        schema top();
        
    public:
        std::string output();

    protected:
        schema *best(int j);
        int worst();

    protected:
        bool set(int index, schema &source);
        std::vector<std::string> split(std::string source);
        
    protected:
        void makeNull();
        void cleanup();
    };
};

#endif