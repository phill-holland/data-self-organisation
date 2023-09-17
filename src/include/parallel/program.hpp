#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"

#ifndef _PARALLEL_PROGRAM
#define _PARALLEL_PROGRAM

namespace organisation
{    
    namespace parallel
    {
        class dimensions
        {
            int length;

        public:
            int width,height,depth;

        public:
            dimensions(int w = 0, int h = 0, int d = 0) 
            {
                width = w;
                height = h;
                depth = d;

                length = width * height * depth;
            }            

            int size() { return length; }
        };

        class program
        {
            ::parallel::device *dev;

            int *deviceValues;
            int *deviceGates;
            int *deviceClient;

        private:
            bool init;

        public:
            program(::parallel::device &dev, dimensions dim, int clients) { makeNull(); reset(dev, dim, clients); }
            ~program() { cleanup(); }

            bool initalised() { return init; }
            void reset(::parallel::device &dev, dimensions dim, int clients);

            void run(::parallel::queue *q = NULL);

        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif