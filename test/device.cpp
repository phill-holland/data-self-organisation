#include <gtest/gtest.h>
#include "parallel/program.hpp"
#include "general.h"
#include "data.h"
#include "schema.h"

organisation::program getTestProgram(organisation::data &d, int width, const int height, const int depth)
{
    organisation::program p(width, height, depth);
    
    std::vector<std::string> stringsA = organisation::split("daisy daisy give me your answer do .");
    std::vector<std::string> stringsB = organisation::split("I eat monkeys for breakfast with jam .");
    d.add(stringsA);
    d.add(stringsB);

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

TEST(BasicProgramExecutionParallel, BasicAssertions)
{    
    const int width = 10, height = 10, depth = 10;
    const int clients = 200;
    const int epochs = 2;

    organisation::data d;

    organisation::schema s1(width, height, depth);

    s1.prog = getTestProgram(d, width, height, depth);

	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    organisation::parallel::parameters parameters(width, height, depth);
    parameters.epochs = epochs;
    
    organisation::parallel::program p_program(*dev, parameters, clients);

    p_program.clear(q);

    std::vector<organisation::schema*> source = { &s1, &s1 };
    p_program.copy(source.data(), source.size(), q);

    int x1 = (width / 2);
    int y1 = (height / 2);
    int z1 = (depth / 2);

    int x2 = width - 1;
    int y2 = height - 1;
    int z2 = depth - 1;

    organisation::vector w {0,1,0};
    std::vector<sycl::float4> positions = { { x1, y1, z1, w.encode() }, { x1 - 2, y1, z1, w.encode() } };
    
    p_program.set(positions, q);

    p_program.run(q);

    std::vector<organisation::parallel::output> results = p_program.get(d, q);
        
    std::string expected1("daisy daisy give me your answer do .");
    std::string expected2("I eat monkeys for breakfast with jam .");

    for(int i = 0; i < clients; ++i)
    {
        EXPECT_EQ(results[0].values[0], expected1);
        EXPECT_EQ(results[0].values[1], expected2);
    }
}