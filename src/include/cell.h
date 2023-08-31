#include "vector.h"
#include <string>
#include <random>

#ifndef _ORGANISATION_CELL
#define _ORGANISATION_CELL

namespace organisation
{    
    class cell
    {   
        static std::mt19937_64 generator;

    public:
        int value;
        uint32_t in, out;

    public:
        cell() { clear(); }

        void clear(); 
        void generate(int value);     
        void mutate();   

        bool is_input(vector source);
        std::vector<vector> outputs();

    public:
        static cell cross(cell &a, cell &b);

    protected:
        int get();
    };
};

#endif