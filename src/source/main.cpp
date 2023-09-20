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
it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";*/

void run()//int rerun = 1)
{         
    auto strings = organisation::split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do ." };//, "I'm half crazy for the love of you ." };

	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    const int width = 5, height = 5, depth = 5, in = 15, out = 10;
    const int size = 1000, clients = 800;

    //organisation::parallel::parameters parameters(width, height, depth, in, out);

    organisation::populations::parameters settings;
    settings.params = organisation::parallel::parameters(width, height, depth, in, out);
    settings.dev = dev;
    settings.q = q;
    settings.expected = expected;
    settings.mappings = organisation::data(strings);
    settings.clients = clients;
    settings.size = size;

    int epochs = expected.size();

    //organisation::data data(strings);
    organisation::populations::population p(settings);//data, expected, 1000);//3000);//2000);//1000);
    
    const int iterations = 300;

    //for(int j = 0; j < rerun; ++j)
    //{        
        int actual = 0;

        p.clear();
        p.generate();

        organisation::schema best = p.go(expected, actual, iterations);

        for(int i = 0; i < epochs; ++i)
        {
            organisation::history history;

            std::string output = best.run(i, expected[i], settings.mappings, &history);
            std::cout << "\r\n" << output << "\r\n";
            std::cout << history.get(settings.mappings);

            if(actual <= iterations) 
            {
                std::string filename("output");
                filename += std::to_string(i);
                filename += std::string(".csv");

                history.append(filename, settings.mappings);
            }
        }        
    //}
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

organisation::program getTestProgram1(organisation::data &d, int width, const int height, const int depth)
{
    organisation::program p(width, height, depth);

    //std::vector<std::string> expected = organisation::split("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split("daisy daisy give me your answer do .");
    d.add(strings);
    //organisation::data d(strings);

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

    return p;
}

organisation::program getTestProgram2(organisation::data &d, const int width, const int height, const int depth)
{
    organisation::program p(width, height, depth);

    //std::vector<std::string> expected = organisation::split("daisy daisy give me");
    
    std::vector<std::string> strings = organisation::split("daisy daisy give me");
    //organisation::data d(strings);
    d.add(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up  };
    std::vector<organisation::vector> out = { down, right, down, right };

    int x = width - 1;
    int y = height - 1;
    int z = depth - 1;

    int magnitude = 2;

    for(int i = 0; i < strings.size(); ++i)
    {
        int value = d.map(strings[i]);
        
        organisation::vector out1 = out[i];

        p.set(in[i], out1, magnitude, x, y, z);
        p.set(value, x, y, z);

        x += (out1.x * magnitude);
        y += (out1.y * magnitude);
        z += (out1.z * magnitude);
    }

    return p;
}

bool BasicProgramExecutionParallel()
{    
    const int width = 10, height = 10, depth = 10;
    organisation::data d;

    organisation::program p1 = getTestProgram1(d, width, height, depth);
    organisation::program p2 = getTestProgram2(d, width, height, depth);

    std::vector<std::string> devices = ::parallel::device::enumerate();
    for(std::vector<std::string>::iterator it = devices.begin(); it < devices.end(); ++it)
    {
        std::cout << *it << "\r\n";
    }
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    const int clients = 2;

    organisation::parallel::parameters parameters(width, height, depth);
    organisation::parallel::program p_program(*dev, parameters, clients);

    p_program.clear(q);

    std::vector<organisation::program> source = { p1, p2 };
    //p_program.copy(source, q);

    int x1 = (width / 2);
    int y1 = (height / 2);
    int z1 = (depth / 2);

    int x2 = width - 1;
    int y2 = height - 1;
    int z2 = depth - 1;


    organisation::vector w {0,1,0};
    std::vector<sycl::float4> positions = { { x1, y1, z1, w.encode() }, { x2, y2, z2, w.encode() } };

    //std::cout << "main input " << x << "," << y << "," << z << "," << w.encode() << "\r\n";
    p_program.set(positions, q);

    std::cout << "run\r\n";

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(q);
    
    //std::vector<std::string> expected1 = organisation::split("daisy daisy give me your answer do .");
    std::string expected1("daisy daisy give me your answer do .");
    std::string expected2("daisy daisy give me");

    std::string results1 = d.get(results[0].values);
    std::cout << "RESULT " << results1 << "\r\n";
    if(expected1 == results1) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";

    std::string results2 = d.get(results[1].values);
    std::cout << "RESULT " << results2 << "\r\n";
    if(expected2 == results2) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";

//std::cout << d.get(results[1].values) << "\r\n";
    return true;
}

bool BasicProgramExecutionParallelBatch()
{    
    const int width = 10, height = 10, depth = 10;
    organisation::data d;

    organisation::program p1 = getTestProgram1(d, width, height, depth);
    organisation::program p2 = getTestProgram2(d, width, height, depth);

    std::vector<std::string> devices = ::parallel::device::enumerate();
    for(std::vector<std::string>::iterator it = devices.begin(); it < devices.end(); ++it)
    {
        std::cout << *it << "\r\n";
    }
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    const int clients = 5000;

    organisation::parallel::parameters parameters(width, height, depth);
    organisation::parallel::program p_program(*dev, parameters, clients);

    p_program.clear(q);

    int x1 = (width / 2);
    int y1 = (height / 2);
    int z1 = (depth / 2);

    int x2 = width - 1;
    int y2 = height - 1;
    int z2 = depth - 1;

    organisation::vector w {0,1,0};

    std::vector<organisation::program> source;
    std::vector<sycl::float4> positions;

    for(int i = 0; i < clients; ++i)
    { 
        if(i % 2 == 0) 
        {
            source.push_back(p1);
            positions.push_back({ x1, y1, z1, w.encode() });
        }
        else 
        {
            source.push_back(p2);       
            positions.push_back({ x2, y2, z2, w.encode() });
        }
    }

    //p_program.copy(source, q);
    
    p_program.set(positions, q);

    std::cout << "run\r\n";

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(q);
    
    std::string expected1("daisy daisy give me your answer do .");
    std::string expected2("daisy daisy give me");

    for(int i = 0; i < clients; ++i)
    { 
        std::string value = d.get(results[i].values);

        if(i % 2 == 0) 
        {            
            if(expected1 != value) return false;
        }
        else
        {
            if(expected2 != value) return false;
        }
    }

    return true;
}

int main(int argc, char *argv[])
{  
    //if(BasicProgramExecutionParallelBatch()) std::cout << "OK\r\n";
    //else std::cout << "NOT OK\r\n";

    run();//200);

    return 0;
}