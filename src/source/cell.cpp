#include "cell.h"
#include <stdlib.h>
#include <math.h>

std::mt19937_64 organisation::cell::generator(std::random_device{}());

void organisation::cell::clear()
{
    value = -1;
    routes.clear();
    //in_gates.clear();
    /*
    for(int i = 0; i < routes.size(); ++i)
    {
        routes[i].clear();
    } */   
}

void organisation::cell::generate(int value)
{
    clear();

    this->value = value;
    routes.generate();
}

void organisation::cell::mutate(int max)
{
    int j = (std::uniform_int_distribution<int>{0, 2})(generator);     

    if(j == 0) value = -1;
    else if (j == 1)
    {
        value = (std::uniform_int_distribution<int>{0, max - 1})(generator);          
    }
    else if (j == 2)
    {
        routes.mutate();
        /*
        int k = (std::uniform_int_distribution<int>{0, 26})(generator); 
        int m = (std::uniform_int_distribution<int>{0, 26})(generator); 
                
        //if(routes[k].values[m].magnitude == -1) 
        if(routes[k].get(m).magnitude == -1)
        {
            int magnitude = (std::uniform_int_distribution<int>{1, MAGNITUDE})(generator);
            routes[k].set(m, gate(magnitude));
            //routes[k].values[m].magnitude = magnitude;
        }
        else routes[j].set(m, gate(-1));//routes[j].values[m].magnitude = -1;
        */
    }
}

bool organisation::cell::is_input(vector source)
{
    int tx = source.x + 1;
    int ty = source.y + 1;
    int tz = source.z + 1;

	int temp = (abs(tz) * (3 * 3)) + (abs(ty) * 3) + abs(tx);
    
    return !routes.is_empty(temp);
    //return !routes[temp].is_empty();
}

std::vector<organisation::vector> organisation::cell::outputs(vector input)
{
    //std::vector<organisation::vector> result;

    int in = map(input);
    return routes.outputs(in);
    /*
    organisation::gates *source = &routes[in];
    for(int index = 0; index < 27; ++index)
    {
        //if(!source->values[index].is_empty())
        if(!source->get(index).is_empty())
        {
            //int magnitude = source->values[index].magnitude;
            int magnitude = source->get(index).magnitude;

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
    */
    //return result;
}

void organisation::cell::set(vector input, vector output, int magnitude)
{
    int s1 = map(input);
    int s2 = map(output);

    routes.set(s1, s2, gate(magnitude));
    //routes[s1].set(s2, gate(magnitude));
    //routes[s1].values[s2].magnitude = magnitude;
}

std::tuple<bool,bool> organisation::cell::validate(int max)
{
    int in = 0;

    if(!routes.validate(in)) return std::tuple<bool, bool>(false,in <= gates::IN);
    if((value < -1)||(value > max)) return std::tuple<bool, bool>(false,in <= gates::IN);

    return std::tuple<bool, bool>(true, in <= gates::IN);

    /*
    int in = 0;
    for(int i = 0; i < routes.size(); ++i)
    {
        int out = 0;
        std::vector<gate> gates = routes[i].get();
        //for(int j = 0; j < routes[i].values.size(); ++j)
        for(std::vector<gate>::iterator it = gates.begin(); it < gates.end(); ++it)
        {
            //if(routes[i].values[j].magnitude > 0)
            //if(routes[i]
            //{
                ++out;
            //}

            //if(routes[i].values[j].magnitude > MAGNITUDE)
            if(it->magnitude > MAGNITUDE)
                return std::tuple<bool, bool>(false,in <= IN);
        }

        if(out > OUT) std::tuple<bool, bool>(false,in <= IN);

        if(out > 0) ++in;
    }

    if((value < -1)||(value > max)) return std::tuple<bool, bool>(false,in <= IN);

    return std::tuple<bool, bool>(true, in <= IN);
    */
}

bool organisation::cell::equals(const cell &source)
{
    if(value != source.value) return false;
    
    return routes.equals(source.routes);
    /*
    for(int i = 0; i < source.routes.size(); ++i)
    {
        if(!routes[i].equals(source.routes[i])) return false;
    }
    */

    return true;

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