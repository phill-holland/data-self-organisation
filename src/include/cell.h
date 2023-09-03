#include "vector.h"
#include "gate.h"
#include <string>
#include <random>
#include <array>
#include <tuple>

#ifndef _ORGANISATION_CELL
#define _ORGANISATION_CELL

namespace organisation
{        
    class cell
    {    
        static const int IN = 4;
        static const int OUT = 1;
        static const int MAGNITUDE = 10;

        static std::mt19937_64 generator;

    public:
        int value;
        std::array<organisation::gates, 27> routes;

    public:
        cell() { clear(); }

        void clear(); 
        void generate(int value);     
        void mutate(int max);   

        bool is_input(vector source);
        std::vector<vector> outputs(vector input);

        void set(vector input, vector output, int magnitude);

        bool is_empty() { return value < 0; }

        std::tuple<bool,bool> validate(int max);

        bool equals(const cell &source);

    protected:
        int map(vector source);
    };
};

#endif