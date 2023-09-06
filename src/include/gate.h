#include "vector.h"
#include <string>
#include <vector>
#include <random>
#include <array>
#include <unordered_map>

#ifndef _ORGANISATION_GATE
#define _ORGANISATION_GATE

namespace organisation
{        
    class gate
    {
    public:        
        static const int MAGNITUDE = 2;

    public:
        int magnitude;

    public:
        gate() { magnitude = -1; }
        gate(int magnitude) { this->magnitude = magnitude; }

    public:
        void clear() { magnitude = -1; }

        bool is_empty() { return magnitude <= 0; }
    };

    class gates
    {
        std::unordered_map<int, gate> data;

    public:
        static const int IN = 15;
        static const int OUT = 10;//6;//3;

    public:
        gates() { }
        gates(const gates &source) { copy(source); }

        void clear() { data.clear(); }

        bool is_empty() { return data.size() <= 0; }

        int size() { return data.size(); }

        void set(int index, gate g);
        gate get(int index);

        std::vector<organisation::vector> get();
        
    public:
        bool validate(int &count);
        
    public:
        bool equals(const gates &source);  
        void copy(const gates &source);      
    };
};

#endif