#include "vector.h"
#include "gate.h"
#include <string>
#include <random>
#include <array>

#ifndef _ORGANISATION_CELL
#define _ORGANISATION_CELL

namespace organisation
{        
    class cell
    {    
        static const int IN = 2;
        static const int OUT = 2;
        static const int MAGNITUDE = 10;

        static std::mt19937_64 generator;

    public:
        int value;
        std::array<organisation::gates, 27> routes;

    public:
        cell() { clear(); }

        void clear(); 
        void generate(int value);     
        void mutate();   

        bool is_input(vector source);
        std::vector<vector> outputs(vector input);

        void set(vector input, vector output, int magnitude);

        bool is_empty() { return value < 0; }

    protected:
        int map(vector source);
    };
};

#endif