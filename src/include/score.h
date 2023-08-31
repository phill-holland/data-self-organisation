#include <string>
#include <vector>

#ifndef _ORGANISATION_SCORE
#define _ORGANISATION_SCORE

namespace organisation
{
    class score
    {
    //public:
        //const static int length = 13;

        

    //public:
        //std::array<float, length> scores;
        //std::vector<std::string> alphabet;
        float *scores;
        int length;

        bool init;

    public:
        score(int length) { makeNull(); reset(length); }
        ~score() { cleanup(); }


        bool intialised() { return init; }
        void reset(int length);

        void clear();
        void compute(std::string expected, std::string value);

        float sum();

        float get(int index);

        int size() { return length; }

    protected:
        void makeNull() { scores = NULL; }
        void cleanup()
        {
            if(scores != NULL) delete[] scores;
        }
    };
};

#endif