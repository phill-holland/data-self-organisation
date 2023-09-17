#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"

#ifndef _PARALLEL_PROGRAM
#define _PARALLEL_PROGRAM

namespace organisation
{    
    namespace parallel
    {
        class parameters
        {
            int length;

        public:
            int width, height, depth;
            int in, out;
        public:
            parameters(int _width = 5, int _height = 5, int _depth = 5, int _in = 3, int _out = 3) 
            {
                width = _width;
                height = _height;
                depth = _depth;
                
                in = _in;
                out = _out;

                length = _width * _height * _depth;
            }            

            int size() { return length; }
        };

        class program
        {
            ::parallel::device *dev;

            int *deviceValues;
            int *deviceInGates;
            int *deviceOutGates;
            int *deviceClient;

            int *deviceOutput;
            int *deviceOutputEndPtr;

            sycl::float4 *deviceReadPositionsA;
            sycl::float4 *deviceReadPositionsB;
            int *deviceReadPositionsEndPtr;
            // need to duplicate EndPtr buffer to A and B too!

            //int *deviceHasReadPosition;
            //int *hostHasOutput;

            parameters params;

            int clients;
            int length;

        private:
            bool init;

        public:
            program(::parallel::device &dev, parameters settings, int clients) { makeNull(); reset(dev, settings, clients); }
            ~program() { cleanup(); }

            bool initalised() { return init; }
            void reset(::parallel::device &dev, parameters settings, int clients);

            void clear(::parallel::queue *q = NULL);

            void run(std::vector<sycl::float4> positions, ::parallel::queue *q = NULL);

// need pinned memory below
// void copy_batch(std::vector<native::program> programs);

        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif