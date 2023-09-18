#include "gate.h"
#include "vector.h"
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

#ifndef _ORGANISATION_IN
#define _ORGANISATION_IN

namespace organisation
{        
    class in
    {
        static std::mt19937_64 generator;
        std::unordered_map<int, organisation::gates> gates;

    public:
        void clear()
        {
            gates.clear();
        }

        bool is_empty(int index)
        {
            if(gates.find(index) == gates.end()) return true;
            return gates[index].is_empty();
        }

        void set(int in, int out, gate value);     
        gate get(int in, int out);
           
        std::vector<int> pull();
        std::vector<int> pull(int in);

        std::vector<organisation::vector> outputs(int index);
        
        void generate();
        void mutate();

    public:
        bool validate(int &in);
        
    public:
        bool equals(const in &source);   
        void copy(const in &source);     
    };
};

#endif