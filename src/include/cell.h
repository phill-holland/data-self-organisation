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
        static std::mt19937_64 generator;

        in routes;

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

        std::vector<int> pull() { return routes.pull(); }
        std::vector<int> pull(int in) { return routes.pull(in); }

        bool equals(const cell &source);
        void copy(const cell &source);
        
    protected:
        int map(vector source);
    };
};

#endif