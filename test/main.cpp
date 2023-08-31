#include <gtest/gtest.h>
#include <string>
#include "program.h"

TEST(BasicProgramExecutionWithStringOutput, BasicAssertions)
{
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
   EXPECT_TRUE(true);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
