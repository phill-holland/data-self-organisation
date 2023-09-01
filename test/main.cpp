#include <gtest/gtest.h>
#include <string>
#include "program.h"
#include "data.h"
#include "vector.h"

// also check cross function in separate test!
// also in new test, check two directional outputs!
// test magnitude doesn't cause crash when goes off grid!!

TEST(BasicProgramExecution, BasicAssertions)
{
    //GTEST_SKIP();

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

    std::vector<std::string> outputs = split(p.run(0, d));

    EXPECT_EQ(p.count(), 8);
    EXPECT_EQ(outputs, expected);        
}

TEST(BasicProgramCross, BasicAssertions)
{
    //GTEST_SKIP();

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

    organisation::program p1, p2;

    std::vector<std::string> expected = split("daisy daisy give me your answer do . I'm half crazy for the love of you .");
    
    std::vector<std::string> strings1 = split("daisy daisy give me your answer do .");
    organisation::data d(expected);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in1 = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out1 = { down, right, down, right, down, right, down, right };

    int x = organisation::program::WIDTH - 1;
    int y = organisation::program::HEIGHT - 1;
    int z = organisation::program::DEPTH - 1;

    int magnitude = 1;

    for(int i = 0; i < strings1.size(); ++i)
    {
        int value = d.map(strings1[i]);
        
        organisation::vector _out = out1[i];

        p1.set(in1[i], _out, magnitude, x, y, z);
        p1.set(value, x, y, z);

        x += _out.x;
        y += _out.y;
        z += _out.z;
    }

    // ***
    int offset = (z * (organisation::program::WIDTH * organisation::program::HEIGHT)) + (organisation::program::WIDTH * y) + x;

    std::vector<std::string> strings2 = split("I'm half crazy for the love of you .");

    organisation::vector back = { 0, 0, -1 };
    organisation::vector forward = { 0, 0, 1 };

    std::vector<organisation::vector> in2 = { left, forward, forward, forward, forward, forward, forward, forward, forward };
    std::vector<organisation::vector> out2 = { back, back, back, back, back, back, back, back, back };
    
    for(int i = 0; i < strings2.size(); ++i)
    {
        int value = d.map(strings2[i]);

        organisation::vector _out = out2[i];

        p2.set(in2[i], _out, magnitude, x, y, z);
        p2.set(value, x, y, z);

        x += _out.x;
        y += _out.y;
        z += _out.z;
    }
    
    // ***

    organisation::program result;
    result.cross(p2, p1, offset + 1);

    std::vector<std::string> outputs = split(result.run(0, d));

    EXPECT_EQ(result.count(), 17);
    EXPECT_EQ(outputs, expected);    
    
}

TEST(BasicProgramExecutionWithMagnitude, BasicAssertions)
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

    std::vector<std::string> expected = split("daisy daisy give me");
    
    std::vector<std::string> strings = split("daisy daisy give me");
    organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in = { up, up, left, up  };
    std::vector<organisation::vector> out = { down, right, down, right };

    int x = organisation::program::WIDTH - 1;
    int y = organisation::program::HEIGHT - 1;
    int z = organisation::program::DEPTH - 1;

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

    std::vector<std::string> outputs = split(p.run(0, d));

    EXPECT_EQ(p.count(), 4);
    EXPECT_EQ(outputs, expected);        
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
