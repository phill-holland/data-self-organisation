#include "cell.h"
#include "data.h"
#include <string>
#include <random>
#include <vector>
#include <unordered_map>
#include <tuple>

#ifndef _ORGANISATION_PROGRAM
#define _ORGANISATION_PROGRAM

namespace organisation
{    
    class program
    {
        static const int WIDTH = 100;
        static const int HEIGHT = 100;
        static const int DEPTH = 100;

        static std::mt19937_64 generator;

        cell *cells;
        int length;

        bool init;

    public:
        program() { makeNull(); reset(); }
        program(const program &source) { copy(source); }
        ~program() { cleanup(); }

        bool intialised() { return init; }
        void reset();

    public:
        void clear();
        void generate(data &source);  
        void mutate();
        
        std::string run(int start, data &source);

        int count();

    public:
        void copy(const program &source);

    public:
        void cross(program &a, program &b);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif