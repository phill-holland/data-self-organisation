#include "program.h"
#include "data.h"
#include "score.h"
#include "kdpoint.h"
#include "semaphore.h"
#include <string>
#include <random>
#include <vector>
#include <tuple>
#include <unordered_map>

#ifndef _ORGANISATION_SCHEMA
#define _ORGANISATION_SCHEMA

namespace organisation
{
    class schema
    {
        static std::mt19937_64 generator;
        
        //threading::semaphore::token token;

        //int epochs;

        bool init;

    public:
        program prog;
        std::unordered_map<int, score> scores;
        //score **scores;
        //std::vector<score> scores;


    public:
        schema(int width, int height, int depth) : prog(width, height, depth) { makeNull(); reset(); }        
        schema(const schema &source) { copy(source); }
        ~schema() { cleanup(); }

    public:
        void clear();
        
        bool initalised() { return init; }
        void reset();
        
        void generate(data &source);
    
        int count() { return prog.count(); }

        bool get(dominance::kdtree::kdpoint &destination, int minimum, int maximum);

        float sum();
        
    public:        
        void cross(schema *destination, schema *value);
        void mutate(data &source);

    public:
        std::string run(int epoch, std::string expected, data &source, history *destination = NULL);           

    public:
        void copy(const schema &source);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif