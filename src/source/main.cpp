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

std::string source = R"(daisy daisy give me your answer do .
I'm half crazy for the love of you .)";
/*it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";*/

organisation::schema run()//int rerun = 1)
{         
    auto strings = organisation::split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do .", "I'm half crazy for the love of you ." };

	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    const int width = 5, height = 5, depth = 5, in = 15, out = 10;
    const int size = 3000, clients = 2500;
    const int epochs = expected.size();
//1000, 800
    //organisation::parallel::parameters parameters(width, height, depth, in, out);

    organisation::populations::parameters settings;
    settings.params = organisation::parallel::parameters(width, height, depth, in, out);
    settings.dev = dev;
    settings.q = q;
    settings.expected = expected;
    settings.mappings = organisation::data(strings);
    settings.clients = clients;
    settings.size = size;
    settings.params.epochs = epochs;

    

    //organisation::data data(strings);
    organisation::populations::population p(settings);//data, expected, 1000);//3000);//2000);//1000);
    
    const int iterations = 300; // 300

    //for(int j = 0; j < rerun; ++j)
    //{        
        int actual = 0;

        p.clear();
        p.generate();

        organisation::schema best(settings.params.width,settings.params.height,settings.params.depth);
        best.copy(p.go(expected, actual, iterations));

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

    return best;    
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

organisation::program getTestProgram3(organisation::data &d, int width, const int height, const int depth)
{
    organisation::program p(width, height, depth);

    //std::vector<std::string> expected = organisation::split("daisy daisy give me your answer do .");
    
    std::vector<std::string> stringsA = organisation::split("daisy daisy give me your answer do .");
    std::vector<std::string> stringsB = organisation::split("I eat monkeys for breakfast with jam .");
    d.add(stringsA);
    d.add(stringsB);
    //organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out = { down, right, down, right, down, right, down, right };

    int x = (width / 2);
    int y = (height / 2);
    int z = (depth / 2);

    int magnitude = 1;

    for(int i = 0; i < stringsA.size(); ++i)
    {
        int valueA = d.map(stringsA[i]);
        int valueB = d.map(stringsB[i]);

        organisation::vector out1 = out[i];

        p.set(in[i], out1, magnitude, x, y, z);
        p.set(valueA, x, y, z);

        p.set(in[i], out1, magnitude, x - 2, y, z);
        p.set(valueB, x - 2, y, z);

        x += out1.x;
        y += out1.y;
        z += out1.z;
    }

    return p;
}

bool BasicProgramExecutionParallel()
{    
    const int width = 10, height = 10, depth = 10;
    const int clients = 2;
    const int epochs = 2;

    organisation::data d;

    organisation::schema s1(width, height, depth);
    //organisation::schema s2(width, height, depth);

    //organisation::program p1 = getTestProgram1(d, width, height, depth);
    //organisation::program p2 = getTestProgram2(d, width, height, depth);

    s1.prog = getTestProgram3(d, width, height, depth);
    //s2.prog = getTestProgram2(d, width, height, depth);

    std::vector<std::string> devices = ::parallel::device::enumerate();
    for(std::vector<std::string>::iterator it = devices.begin(); it < devices.end(); ++it)
    {
        std::cout << *it << "\r\n";
    }
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    organisation::parallel::parameters parameters(width, height, depth);
    parameters.epochs = epochs;
    
    organisation::parallel::program p_program(*dev, parameters, clients);

    p_program.clear(q);

    //std::vector<organisation::program> source = { p1, p2 };
    //std::vector<organisation::schema*> source = { &s1, &s2 };
    std::vector<organisation::schema*> source = { &s1, &s1 };
    p_program.copy(source.data(),source.size(),q);//source, q);

    int x1 = (width / 2);
    int y1 = (height / 2);
    int z1 = (depth / 2);

    int x2 = width - 1;
    int y2 = height - 1;
    int z2 = depth - 1;


    organisation::vector w {0,1,0};
    std::vector<sycl::float4> positions = { { x1, y1, z1, w.encode() }, { x1 - 2, y1, z1, w.encode() } };

    //std::cout << "main input " << x << "," << y << "," << z << "," << w.encode() << "\r\n";
    p_program.set(positions, q);

    std::cout << "run\r\n";

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(d, q);
    
    
    //std::vector<std::string> expected1 = organisation::split("daisy daisy give me your answer do .");
    std::string expected1("daisy daisy give me your answer do .");
    std::string expected2("I eat monkeys for breakfast with jam .");

    //std::string out1 = s1.run(0, expected1, d, NULL);
    //std::cout << "out1 " << out1 << "\r\n";

    //std::string out2 = s2.run(0, expected2, d, NULL);
    //std::cout << "out2 " << out2 << "\r\n";

    std::string results1 = results[0].values[0];
    std::cout << "RESULT " << results1 << "\r\n";
    if(expected1 == results1) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";

    std::string results2 = results[1].values[0];
    std::cout << "RESULT " << results2 << "\r\n";
    if(expected1 == results2) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";

    results1 = results[0].values[1];
    std::cout << "RESULT " << results1 << "\r\n";
    if(expected2 == results1) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";

    results2 = results[1].values[1];
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

    organisation::schema s1(width,height,depth);
    s1.prog = p1;

    organisation::schema s2(width,height,depth);
    s2.prog = p2;

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

    std::vector<organisation::schema*> source;
    std::vector<sycl::float4> positions;

    for(int i = 0; i < clients; ++i)
    { 
        if(i % 2 == 0) 
        {
            source.push_back(&s1);
            positions.push_back({ x1, y1, z1, w.encode() });
        }
        else 
        {
            //organisation::schema s2(width,height,depth);
            //s2.prog = p2;
            source.push_back(&s2);       
            positions.push_back({ x2, y2, z2, w.encode() });
        }
    }

    p_program.copy(source.data(),source.size(), q);
    
    p_program.set(positions, q);

    std::cout << "run\r\n";

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(d,q);
    
    std::string expected1("daisy daisy give me your answer do .");
    std::string expected2("daisy daisy give me");

    for(int i = 0; i < clients; ++i)
    { 
        std::string value = results[i].values[0];

        if(i % 2 == 0) 
        {            
            if(expected1 != value) 
            {
                std::cout << "wrong " << i << " [" << expected1 << "] [" << value << "]\r\n";
                return false;
            }
        }
        else
        {
            if(expected2 != value) 
            {
                std::cout << "wrong " << i << " [" << expected2 << "] [" << value << "]\r\n";
                return false;
            }
        }
    }

    return true;
}

bool BasicProgramExecutionParallelSingle(organisation::schema &s1,organisation::data d,const int width, int height, int depth)
{          
    //organisation::schema s1(width, height, depth);
    //organisation::schema s2(width, height, depth);

    //organisation::program p1 = getTestProgram1(d, width, height, depth);
    //organisation::program p2 = getTestProgram2(d, width, height, depth);

    //s1.prog = p1;//getTestProgram1(d, width, height, depth);
    //s2.prog = getTestProgram2(d, width, height, depth);

    std::vector<std::string> devices = ::parallel::device::enumerate();
    for(std::vector<std::string>::iterator it = devices.begin(); it < devices.end(); ++it)
    {
        std::cout << *it << "\r\n";
    }
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    const int clients = 1;

    organisation::parallel::parameters parameters(width, height, depth, 15, 10);
    organisation::parallel::program p_program(*dev, parameters, clients);

    p_program.clear(q);

    //std::vector<organisation::program> source = { p1, p2 };
    std::vector<organisation::schema*> source = { &s1 };
    p_program.copy(source.data(),source.size(),q);//source, q);

    int x1 = (width / 2);
    int y1 = (height / 2);
    int z1 = (depth / 2);

    int x2 = width - 1;
    int y2 = height - 1;
    int z2 = depth - 1;


    organisation::vector w {0,1,0};
    std::vector<sycl::float4> positions = { { x1, y1, z1, w.encode() } };

    //std::cout << "main input " << x << "," << y << "," << z << "," << w.encode() << "\r\n";
    p_program.set(positions, q);

    std::cout << "run\r\n";

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(d,q);
    
    
    //std::vector<std::string> expected1 = organisation::split("daisy daisy give me your answer do .");
    std::string expected1("daisy daisy give me your answer do .");
    //std::string expected2("daisy daisy give me");

    std::string out1 = s1.run(0, expected1, d, NULL);
    std::cout << "CPU result [" << out1 << "]\r\n";

    //std::string out2 = s2.run(0, expected2, d, NULL);
    //std::cout << "out2 " << out2 << "\r\n";

    std::string results1 = results[0].values[0];
    std::cout << "GPU RESULT [" << results1 << "]\r\n";
    if(expected1 == results1) std::cout << "OK\r\n";
    else std::cout << "NOT OK\r\n";
/*
for(int i = 0; i <results.size(); ++i)
{
    for(int j = 0; j < results[i].values.size(); ++j)
    {
        std::cout << results[i].values[j] << " ";
    }
    std::cout << "\r\n";
}
*/
   // std::string results2 = d.get(results[1].values);
    //std::cout << "RESULT " << results2 << "\r\n";
    //if(expected2 == results2) std::cout << "OK\r\n";
    //else std::cout << "NOT OK\r\n";

//std::cout << d.get(results[1].values) << "\r\n";
    return true;
}

int main(int argc, char *argv[])
{  
    //BasicProgramExecutionParallel();
    //return 0;
   // if(BasicProgramExecutionParallelBatch()) std::cout << "OK\r\n";
   // else std::cout << "NOT OK\r\n";
   // return 0;
/*
const int width = 10, height = 10, depth = 10;
organisation::data d;
organisation::schema s1(width, height, depth);
organisation::program p1 = getTestProgram1(d,width,height,depth); //(10,10,10);
s1.prog = p1;
    BasicProgramExecutionParallelSingle(s1,d,width,height,depth);
*/
    organisation::schema s1 = run();//200);
    //s2.prog.save("prog3.txt");
    
    //return 0;

const int width = 5, height = 5, depth = 5;
//organisation::schema s1(width,height,depth);

std::vector<std::string> strings = organisation::split("daisy daisy give me your answer do .");
organisation::data d(strings);

//s1.generate(d);
s1.prog.save("run.txt");
//s1.clear();
//s1.prog.load("prog3.txt");
//s1.prog.save("prog2.txt");
//organisation::schema s1(width, height, depth);
//organisation::program p1 = getTestProgram1(d,width,height,depth); //(10,10,10);
//s1.prog = p1;
    BasicProgramExecutionParallelSingle(s1,d,width,height,depth);

    return 0;
}

/*
auto is_dominant = [](int *a, int *b) 
	{ 
		const int dimensions = 2;
		bool any = false;
		for (int i = 0; i < dimensions; ++i)
		{
			//Log << a[i];
			// modify kdtree for this condition, between left and right insert
			if(a[i] > b[i]) return false;
			any |= (a[i] < b[i]);
			//if((*epochs)[i]->results.score > (*source.epochs)[i]->results.score) return false;
			//any |= ((*epochs)[i]->results.score < (*source.epochs)[i]->results.score);
		}

		return any;
	};

*/