#include "vector.h"
#include "gate.h"
#include "in.h"
#include <string>
#include <random>
#include <array>
#include <tuple>
#include <unordered_map>

#ifndef _ORGANISATION_CELL
#define _ORGANISATION_CELL

namespace organisation
{        
    class cell
    {    
        //static const int IN = 4;
        //static const int OUT = 1;
        //static const int MAGNITUDE = 2;

        static std::mt19937_64 generator;

        in routes;
        //std::unordered_map<int, organisation::gates> in_gates;
        //std::array<organisation::gates, 27> routes;

    public:
        int value;        

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