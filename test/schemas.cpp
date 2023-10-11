#include <gtest/gtest.h>
#include "schemas.h"

TEST(SchemasCrowdedDistance, BasicAssertions)
{
    int points[][2] = { {97,23},{55,77},{34,76},{80,60},{99,04},{81,05},{05,81},{30,79},{15,80},{70,65},
                        {90,40},{40,30},{30,40},{20,60},{60,50},{20,20},{30,01},{60,40},{70,25},{44,62},
                        {55,55},{55,10},{15,45},{83,22},{76,46},{56,32},{45,55},{10,70},{10,30},{97,23}
                      };
    
	const int count = 30;

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