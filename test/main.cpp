#include <gtest/gtest.h>
#include <string>
#include "program.h"
#include "data.h"
#include "vector.h"

TEST(BasicProgramExecution, BasicAssertions)
{
    auto split = [](std::string source)
    {
        std::vector<std::string> result;
        std::string temp; 

        for(auto &ch: source)
        {
            if((ch != ' ')&&(ch != 10)&&(ch != 13))
            {
                temp += ch;
            }
            else
            {
                if(temp.size() > 0)
                {
                    result.push_back(temp);
                    temp.clear();
                }
            }
        }

        if(temp.size() > 0) result.push_back(temp);
        
        return result;
    };

    organisation::program p;

    std::vector<std::string> expected = split("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = split("daisy daisy give me your answer do .");
    organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out = { down, right, down, right, down, right, down, right };

    int x = organisation::program::WIDTH - 1;
    int y = organisation::program::HEIGHT - 1;
    int z = organisation::program::DEPTH - 1;

    for(int i = 0; i < strings.size(); ++i)
    {
        int value = d.map(strings[i]);
        
        organisation::vector out1 = out[i];

        p.set(in[i], 0, x, y, z);
        p.set(out1,  1, x, y, z);

        p.set(value, x, y, z);

        x += out1.x;
        y += out1.y;
        z += out1.z;
    }

    std::vector<std::string> outputs = split(p.run(0, d));

    EXPECT_EQ(outputs, strings);    


    // check count too!!

    // also check cross function in separate test!
    // also in new test, check two directional outputs!

    /*
    polymorphic::program p;

    polymorphic::vars::variable v;
    
    v.id = 1;
    v.type = 0;

    p.variables.values.push_back(v);

    std::string expected("hello world!");

    polymorphic::instrs::instruction i1;

    i1.type = 1;
    i1.variables.push_back(v);
    i1.parameters.push_back(expected);

    polymorphic::instrs::instruction i2;

    i2.type = 2;
    i2.variables.push_back(v);

    p.instructions.values.push_back(i1);
    p.instructions.values.push_back(i2);

    std::tuple<std::string, bool, int, int> result = p.run();

    EXPECT_TRUE(expected == std::get<0>(result));
    */
   //EXPECT_TRUE(true);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
