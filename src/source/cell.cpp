#include "cell.h"
#include <stdlib.h>
#include <math.h>

std::mt19937_64 organisation::cell::generator(std::random_device{}());

void organisation::cell::clear()
{
    value = -1;
    for(int i = 0; i < routes.size(); ++i)
    {
        routes[i].clear();
    }    
}

void organisation::cell::generate(int value)
{
    this->value = value;

    int in = (std::uniform_int_distribution<int>{0, IN})(generator);     

    for(int i = 0; i < in; ++i)
    {
        int k = (std::uniform_int_distribution<int>{0, 26})(generator);         
        int out = (std::uniform_int_distribution<int>{0, OUT})(generator);     
        
        for(int j = 0; j < out; ++j)
        {
            int magnitude = (std::uniform_int_distribution<int>{1, MAGNITUDE})(generator);
            int m = (std::uniform_int_distribution<int>{0, 26})(generator);         

            routes[k].values[m].magnitude = magnitude;
        }
    }
}

void organisation::cell::mutate()
{
    int j = (std::uniform_int_distribution<int>{0, 2})(generator);     

    if(j == 0) value = -1;
    else if (j == 1)
    {
        if(value > 1)
        {
            value = (std::uniform_int_distribution<int>{0, value - 1})(generator);     
        } else value = 0;
    }
    else if (j == 2)
    {
        int k = (std::uniform_int_distribution<int>{0, 26})(generator); 
        int m = (std::uniform_int_distribution<int>{0, 26})(generator); 
                
        if(routes[k].values[m].magnitude == -1) 
        {
            int magnitude = (std::uniform_int_distribution<int>{1, MAGNITUDE})(generator);
            routes[k].values[m].magnitude = magnitude;
        }
        else routes[j].values[m].magnitude = -1;
    }
}

bool organisation::cell::is_input(vector source)
{
    int tx = source.x + 1;
    int ty = source.y + 1;
    int tz = source.z + 1;

	int temp = (abs(tz) * (3 * 3)) + (abs(ty) * 3) + abs(tx);
    
    return !routes[temp].is_empty();
}

std::vector<organisation::vector> organisation::cell::outputs(vector input)
{
    std::vector<organisation::vector> result;

    int in = map(input);
    organisation::gates *source = &routes[in];
    for(int index = 0; index < 27; ++index)
    {
        if(!source->values[index].is_empty())
        {
            int magnitude = source->values[index].magnitude;

            vector temp;

            div_t r = div(index, 9);
            temp.z = (float)r.quot - 1L;

            div_t j = div(r.rem, 3);
            temp.y = (float)j.quot - 1L;
            temp.x = (float)j.rem - 1L;

            if((temp.x != 0)||(temp.y != 0)||(temp.z != 0))
            {
                temp.x *= magnitude;
                temp.y *= magnitude;
                temp.z *= magnitude;
                temp.w = magnitude;
                
                result.push_back(temp);
            }
        }
    }

    return result;
}

void organisation::cell::set(vector input, vector output, int magnitude)
{
    int s1 = map(input);
    int s2 = map(output);

    routes[s1].values[s2].magnitude = magnitude;
}

int organisation::cell::map(vector source)
{
    int tx = source.x + 1;
	int ty = source.y + 1;
	int tz = source.z + 1;

	if ((tx < 0) || (tx > 2)) return 0;
	if ((ty < 0) || (ty > 2)) return 0;
	if ((tz < 0) || (tz > 2)) return 0;

	return (abs(tz) * (3L * 3L)) + (abs(ty) * 3L) + abs(tx);
}