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

        void set(int type, vector source);

        bool is_empty() { return value < 0; }
        
    protected:
        int get();
    };
};

#endif