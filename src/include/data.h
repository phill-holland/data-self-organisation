#include <string>
#include <vector>
#include <unordered_map>

#ifndef _ORGANISATION_DATA
#define _ORGANISATION_DATA

namespace organisation
{    
    class data
    {
        std::unordered_map<std::string, int> forward;
        std::unordered_map<int, std::string> reverse;

        int max;

    public:
        data(std::vector<std::string> &source) { reset(source); }

    public:
        void reset(std::vector<std::string> &source);        

        int maximum() { return max; }
        
        std::string map(int value);
        std::string get(std::vector<int> &source);        
    };
};

#endif