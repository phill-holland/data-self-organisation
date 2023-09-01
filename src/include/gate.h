#include "vector.h"
#include <string>
#include <random>
#include <array>

#ifndef _ORGANISATION_GATE
#define _ORGANISATION_GATE

namespace organisation
{        
    class gate
    {
    public:
        int magnitude;

    public:
        gate() { magnitude = -1; }

    public:
        void clear() { magnitude = -1; }

        bool is_empty() { return magnitude < 0; }
    };

    class gates
    {
    public:
        std::array<gate,27> values;

    public:
        void clear()
        {
            for(int i = 0; i < values.size(); ++i)
            {
                values[i].clear();
            }
        }

        bool is_empty()
        {
            for(int i = 0; i < values.size(); ++i)
            {
                if(!values[i].is_empty()) return false;
            }

            return true;
        }
    };
};

#endif