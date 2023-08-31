#include "cell.h"
#include <stdlib.h>
#include <math.h>

std::mt19937_64 organisation::cell::generator(std::random_device{}());

void organisation::cell::clear()
{
    value = 0;
    in = 0;
    out = 0;
}

void organisation::cell::generate(int value)
{
    this->value = value;

    in = get();
    out = get();
}

void organisation::cell::mutate()
{
    int j = (std::uniform_int_distribution<int>{0, 3})(generator);     

    if(j == 0) value = 0;
    else if (j == 1)
    {
        if(value > 1)
        {
            value = (std::uniform_int_distribution<int>{0, value - 1})(generator);     
        } else value = 0;
    }
    else if (j == 2)
    {
        int k = (std::uniform_int_distribution<int>{0, 27})(generator); 
        in = in ^ ((uint32_t)1 << k);
    }
    else if (j == 3)
    {
        int k = (std::uniform_int_distribution<int>{0, 27})(generator); 
        out = out ^ ((uint32_t)1 << k);
    }
}

bool organisation::cell::is_input(vector source)
{
    int tx = source.x + 1;
    int ty = source.y + 1;
    int tz = source.z + 1;

	int temp = (abs(tz) * (3 * 3)) + (abs(ty) * 3) + abs(tx);
    return ((in && (1 << temp)) == (1 << temp));
}

std::vector<organisation::vector> organisation::cell::outputs()
{
    std::vector<organisation::vector> result;

    for(int index = 0; index < 27; ++index)
    {
        if(out & (1 << index) == (1 << index))
        {
            vector temp;

            div_t r = div(index, 9);
            temp.z = (float)r.quot - 1L;

            div_t j = div(r.rem, 3);
            temp.y = (float)j.quot - 1L;
            temp.x = (float)j.rem - 1L;

            if((temp.x != 0)||(temp.y != 0)||(temp.z != 0))
                result.push_back(temp);
        }
    }

    return result;
}

organisation::cell organisation::cell::cross(cell &a, cell &b)
{
    organisation::cell result;

    return result;
}

int organisation::cell::get()
{    
    int j = (std::uniform_int_distribution<int>{0, 4})(generator);     
    int result = 0;

    for(int i = 0; i < j; ++i)
    {
        int k = (std::uniform_int_distribution<int>{0, 27})(generator); 
        result = result ^ ((uint32_t)1 << k);
    }

    return result;
}
