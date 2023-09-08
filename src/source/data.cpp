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

    if(source.begin() != source.end()) 
        result = map(*source.begin());

    for(std::vector<int>::iterator it = source.begin() + 1; it < source.end(); ++it)
    {
        result += std::string(" ") + map(*it);
    }

    return result;
}

void organisation::data::copy(const data &source)
{
    clear();

    for(auto &f: source.forward)
    {
        forward[f.first] = f.second;
    }

    for(auto &r: source.reverse)
    {
        reverse[r.first] = r.second;
    }

    max = source.max;
}