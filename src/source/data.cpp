#include "data.h"

void organisation::data::reset(std::vector<std::string> &source)
{
    max = 0;
    for(std::vector<std::string>::iterator it = source.begin(); it != source.end(); ++it) 
	{
        if(forward.find(*it) == forward.end())
        {
            forward[*it] = max;
            reverse[max] = *it;
            ++max;
        }
    }
}

std::string organisation::data::map(int value)
{
    std::string result;

    if(reverse.find(value) != reverse.end())
    {
        result = reverse[value];
    }

    return result;
}
        
int organisation::data::map(std::string value)
{
    int result = 0;

    if(forward.find(value) != forward.end())
    {
        result = forward[value];
    }

    return result;
}

std::string organisation::data::get(std::vector<int> &source)
{
    std::string result;

    for(std::vector<int>::iterator it = source.begin(); it != source.end(); ++it)
    {
        result += map(*it) + std::string(" ");
    }

    return result;
}