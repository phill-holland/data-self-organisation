#include <vector>

#ifndef _DOMINANCE_KDTREE_KDPOINT
#define _DOMINANCE_KDTREE_KDPOINT

namespace dominance
{
    namespace kdtree
    {
        class kdtree;

        class kdpoint
        {
            friend class kdtree;

            bool init;

            std::vector<long> values;    
            std::vector<long> min, max;

            long dimensions;

            kdpoint *left, *right;
            
        public:
            long identity;

        public:
            kdpoint(long dimensions) { makeNull(); reset(dimensions); }
            ~kdpoint() { cleanup(); }

            bool initalised() { return init; }
            void reset(long dimensions);

            void clear();
            
        public:
            void set(long *points, unsigned long length = 0L);
            bool set(long value, unsigned long dimension);
            void set(long value);

            long get(unsigned long dimension);

            bool dominates(const kdpoint &source);

        public:
            void copy(const kdpoint &source);
            bool equals(const kdpoint &source);
            bool inside(const kdpoint &min, const kdpoint &max);

            bool issame(long value);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif