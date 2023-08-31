#include "program.h"
#include "data.h"
#include "score.h"
#include "kdpoint.h"
#include <string>
#include <random>
#include <vector>
#include <tuple>

#ifndef _ORGANISATION_SCHEMA
#define _ORGANISATION_SCHEMA

namespace organisation
{
    class schema
    {
        static std::mt19937_64 generator;

        program prog;

        int epochs;

        bool init;

    public:
        score **scores;

    public:
        schema(std::vector<int> lengths) { makeNull(); reset(lengths); }        
        ~schema() { cleanup(); }

    public:
        void clear() { prog.clear(); }
        
        bool initalised() { return init; }
        void reset(std::vector<int> lengths);
        
        void generate(data &source);
    
        int count() { return prog.count(); }

        bool get(dominance::kdtree::kdpoint &destination, int minimum, int maximum);

        float sum();
        
    public:        
        void cross(schema &destination, schema &value);
        void mutate();

    public:
        std::string run(int epoch, std::string expected, data &source);           

    public:
        void copy(const schema &source);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif