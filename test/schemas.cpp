#include <gtest/gtest.h>
#include "schemas.h"

TEST(SchemasCrowdedDistance, BasicAssertions)
{
    int points[][2] = { 
                        {97,23},{55,77},{80,60},{99,04},{05,81},{30,79},{15,80},{70,65},{90,40},{97,23}
                      };
    
	const int count = 10;

    const int width = 5, height = 5, depth = 5;

    organisation::schemas population(width, height, depth, count);

    for(int i = 0; i < count; ++i)
    {
        organisation::schema *schema = population.get(i);

        schema->scores[0].set(points[i][0], 0);
        schema->scores[1].set(points[i][1], 0);
    }

    //population.sort(1);
    population.crowded(2);

    ASSERT_EQ(true,true);
}