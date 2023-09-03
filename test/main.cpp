#include <gtest/gtest.h>
#include <string>
#include "program.h"
#include "data.h"
#include "vector.h"
#include "schema.h"

TEST(BasicProgramExecution, BasicAssertions)
{
    GTEST_SKIP();

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
    GTEST_SKIP();

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
    GTEST_SKIP();

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

TEST(BasicProgramGenerationAndMutation, BasicAssertions)
{
    GTEST_SKIP();

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

std::string source = R"(daisy daisy give me your answer do .
I'm half crazy for the love of you .
it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";

    organisation::program p1, p2;
        
    std::vector<std::string> strings = split(source);
    organisation::data d(strings);

    p1.generate(d);

    EXPECT_TRUE(p1.validate(d));

    p2.copy(p1);

    EXPECT_TRUE(p1.equals(p2));

    p2.mutate(d);

    EXPECT_TRUE(p2.validate(d));   
    EXPECT_FALSE(p2.equals(p1));   
}

TEST(BasicProgramScores, BasicAssertions)
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

std::string source = R"(daisy daisy give me your answer do .
but you'll look sweet upon the seat .
)";

    std::string expected1 = "daisy daisy give me your answer do .";
    std::string expected2 = "but you'll look sweet upon the seat .";

    std::vector<std::string> s1 = split(expected1);
    std::vector<std::string> s2 = split(expected2);

    std::vector<int> lengths = { (int)(s1.size() * 2) + 1, (int)(s2.size() * 2) + 1 };

    organisation::schema schema(lengths);

    std::vector<std::string> strings = split(source);
    organisation::data d(strings);

    organisation::vector up = { 0,1,0 } ,left = { 1,0,0 };
    organisation::vector down = { 0,-1,0 } ,right = { -1,0,0 };

    std::vector<organisation::vector> in1 = { up, up, left, up, left, up, left, up };
    std::vector<organisation::vector> out1 = { down, right, down, right, down, right, down, right };

    std::vector<organisation::vector> in2 = { up, left, left, left, left, left, left, left };
    std::vector<organisation::vector> out2 = { right, right, right, right, right, right, right, right };

    int x1 = organisation::program::WIDTH - 1;
    int y1 = organisation::program::HEIGHT - 1;
    int z1 = organisation::program::DEPTH - 1;

    int x2 = organisation::program::WIDTH - 2;
    int y2 = organisation::program::HEIGHT - 1;
    int z2 = organisation::program::DEPTH - 1;

    int magnitude = 1;

    for(int i = 0; i < s1.size(); ++i)
    {
        int value1 = d.map(s1[i]);
        
        organisation::vector out = out1[i];

        schema.prog.set(in1[i], out, magnitude, x1, y1, z1);
        schema.prog.set(value1, x1, y1, z1);

        x1 += out.x;
        y1 += out.y;
        z1 += out.z;

        out = out2[i];

        int value2 = d.map(s2[i]);
        schema.prog.set(in2[i], out, magnitude, x2, y2, z2);
        schema.prog.set(value2, x2, y2, z2);
          
        x2 += out.x;
        y2 += out.y;
        z2 += out.z;
    }

    std::string output1 = schema.run(0, expected1, d);
    std::string output2 = schema.run(1, expected2, d);

    EXPECT_STREQ(output1.c_str(), expected1.c_str());
    EXPECT_STREQ(output2.c_str(), expected2.c_str());

    EXPECT_FLOAT_EQ(schema.sum(), 1.0f);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
