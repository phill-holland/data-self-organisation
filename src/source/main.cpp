#include "population.h"
#include "data.h"
#include "history.h"
#include "general.h"
#include <iostream>
#include <string.h>

#include "fifo.h"
#include "schema.h"
#include "vector.h"

#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/program.hpp"

using namespace std;

std::string source = R"(daisy daisy give me your answer do .)";
/*I'm half crazy for the love of you .)";
/*it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";*/

void run(int rerun = 1)
{     
    auto strings = organisation::split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do ." };//, "I'm half crazy for the love of you ." };

    int epochs = expected.size();

    organisation::data data(strings);
    organisation::population p(data, expected, 1000);//3000);//2000);//1000);
    
    const int iterations = 300;

    for(int j = 0; j < rerun; ++j)
    {        
        int actual = 0;

        p.clear();
        p.generate();
        p.start();

        organisation::schema best = p.go(expected, actual, iterations);

        for(int i = 0; i < epochs; ++i)
        {
            organisation::history history;

            std::string output = best.run(i, expected[i], data, &history);
            std::cout << "\r\n" << output << "\r\n";
            std::cout << history.get(data);

            if(actual <= iterations) 
            {
                std::string filename("output");
                filename += std::to_string(i);
                filename += std::string(".csv");

                history.append(filename, data);
            }
        }        
    }
}

bool BasicProgramExecution()
{
    const int width = 20, height = 20, depth = 20;
    organisation::program p(width, height, depth);

    std::vector<std::string> expected = organisation::split("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split("daisy daisy give me your answer do .");
    organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out = { down, right, down, right, down, right, down, right };

    int x = (width / 2);
    int y = (height / 2);
    int z = (depth / 2);

    int magnitude = 1;

    for(int i = 0; i < strings.size(); ++i)
    {
        int value = d.map(strings[i]);
        
        organisation::vector out1 = out[i];

        p.set(in[i], out1, magnitude, x, y, z);
        p.set(value, x, y, z);

        x += out1.x;
        y += out1.y;
        z += out1.z;
    }

    std::vector<std::string> outputs = organisation::split(p.run(0, d));

    if (p.count() != 8) return false;
    if (outputs != expected) return false;

    return true;
}

bool BasicProgramExecutionParallel()
{
    const int width = 20, height = 20, depth = 20;
    organisation::program p(width, height, depth);

    std::vector<std::string> expected = organisation::split("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split("daisy daisy give me your answer do .");
    organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out = { down, right, down, right, down, right, down, right };

    int x = (width / 2);
    int y = (height / 2);
    int z = (depth / 2);

    int magnitude = 1;

    for(int i = 0; i < strings.size(); ++i)
    {
        int value = d.map(strings[i]);
        
        organisation::vector out1 = out[i];

        p.set(in[i], out1, magnitude, x, y, z);
        p.set(value, x, y, z);

        x += out1.x;
        y += out1.y;
        z += out1.z;
    }

    //std::vector<std::string> outputs = organisation::split(p.run(0, d));

    //if (p.count() != 8) return false;
    //if (outputs != expected) return false;

    std::vector<std::string> devices = ::parallel::device::enumerate();
    for(std::vector<std::string>::iterator it = devices.begin(); it < devices.end(); ++it)
    {
        std::cout << *it << "\r\n";
    }
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    
    organisation::parallel::parameters parameters(width, height, depth);
    organisation::parallel::program p_program(*dev, parameters, 1);

    std::vector<organisation::program> source = { p };
    p_program.copy(source, q);

    organisation::vector w {0,-1,0};
    std::vector<sycl::float4> positions = { { x,y,z,w.encode() } };

    //std::cout << x << "," << y << "," << z << "," << w.encode() << "\r\n";
    p_program.set(positions, q);

    return true;
}

int main(int argc, char *argv[])
{  
    BasicProgramExecutionParallel();
    //run(200);
/*
core::queue::fifo<organisation::schema,10L> fifo;

organisation::schema *temp = new organisation::schema();

fifo.set(*temp);

organisation::schema *moo = new organisation::schema();
fifo.get(*moo);

delete moo;
delete temp;
*/
    return 0;
}