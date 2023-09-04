#include <string>
#include <vector>

#ifndef _ORGANISATION_SCORE
#define _ORGANISATION_SCORE

namespace organisation
{
    class score
    {
        float *scores;
        int length;

        bool init;

    public:
        score(int length) { makeNull(); reset(length); }
        ~score() { cleanup(); }

        bool intialised() { return init; }
        void reset(int length);

        void clear();
        bool compute(std::string expected, std::string value);

        float sum();

        bool set(float value, int index);
        float get(int index);

        int size() { return length; }

    public:
        void copy(const score &source);
        
    protected:
        void makeNull();
        void cleanup();
    };
};

#endif