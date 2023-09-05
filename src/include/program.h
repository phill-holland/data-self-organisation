#include "cell.h"
#include "data.h"
#include "history.h"
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
        static std::mt19937_64 generator;

        cell *cells;
        int length;

        bool init;

    public:
        static const int WIDTH = 5;
        static const int HEIGHT = 5;
        static const int DEPTH = 5;

    public:
        program() { makeNull(); reset(); }
        program(const program &source) { copy(source); }
        ~program() { cleanup(); }

        bool intialised() { return init; }
        void reset();

    public:
        void clear();
        void generate(data &source);  
        void mutate(data &source);
        
        std::string run(int start, data &source, history *destination = NULL);

        int count();

        void set(int value, int x, int y, int z);
        void set(vector input, vector output, int magnitude, int x, int y, int z);

        bool validate(data &source);

    public:
        void copy(const program &source);
        bool equals(const program &source);

    public:
        void cross(program &a, program &b, int middle = -1);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif