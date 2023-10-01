#include <gtest/gtest.h>
#include "parallel/program.hpp"
#include "parallel/front.hpp"
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
    GTEST_SKIP();

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

TEST(BasicFrontSchemaParallel, BasicAssertions)
{    
    const int width = 10, height = 10, depth = 10;
    const int clients = 2;
    const int epochs = 2;

    organisation::schema s1(width, height, depth), s2(width, height, depth);

	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);


    std::string expected1("daisy daisy give me your answer do .");  
    std::string expected2("I'm half crazy for the love of you .");

    std::string result1("daisy give me your answer do .");  
    std::string result2("I'm half crazy the the love of you .");

    std::vector<std::string> expected = { expected1, expected2 };

    int terms = 0;
    for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
    {
        std::vector<std::string> t = organisation::split(*it);
        terms += (t.size() * 2) + 1;
    }

    organisation::parallel::front f(*dev, terms, clients);

    std::vector<std::tuple<std::string,std::string>> source1 = { 
        std::tuple<std::string,std::string>(expected1, result1),
        std::tuple<std::string,std::string>(expected2, result2)
    };

    std::vector<std::tuple<std::string,std::string>> source2 = { 
        std::tuple<std::string,std::string>(expected1, expected1),
        std::tuple<std::string,std::string>(expected2, expected2)
    };

    s1.compute(source1);
    s2.compute(source2);

    f.set(&s1, 0);
    f.set(&s2, 1);

    f.run(q);

    EXPECT_FALSE(f.is_front(0));
    EXPECT_TRUE(f.is_front(1));
}

TEST(BasicFrontTestParallel, BasicAssertions)
{
	::parallel::device *dev = new ::parallel::device(0);
	::parallel::queue *q = new parallel::queue(*dev);

    int points[][2] = { {97,23},{55,77},{34,76},{80,60},{99,04},{81,05},{05,81},{30,79},{15,80},{70,65},
                        {90,40},{40,30},{30,40},{20,60},{60,50},{20,20},{30,01},{60,40},{70,25},{44,62},
                        {55,55},{55,10},{15,45},{83,22},{76,46},{56,32},{45,55},{10,70},{10,30},{97,23}
                      };
    
	const int count = 30;
    const int terms = 2;

    bool results[count];
    for(int i = 0; i < count; ++i) results[i] = false;

    results[0] = true;
    results[1] = true;
    results[3] = true;
    results[4] = true;
    results[6] = true;
    results[7] = true;
    results[8] = true;
    results[9] = true;
    results[10] = true;
    results[29] = true;

    organisation::parallel::front f(*dev, terms, count);

    for(int i = 0; i < count; ++i)
    {
        f.set(points[i],i);
    }

    f.run(q);

    for(int i = 0; i < count; ++i)
    {
        EXPECT_EQ(results[i], f.is_front(i));
    }
}
