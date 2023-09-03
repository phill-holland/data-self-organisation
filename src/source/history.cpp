#include "history.h"

void organisation::history::push(vector source, int value)
{
    values.push_back(std::tuple<vector,int>(source, value));
}

std::string organisation::history::get(data &source)
{
    std::string result;

    for(std::vector<std::tuple<vector,int>>::iterator it = values.begin(); it != values.end(); ++it)
    {
        vector a = std::get<0>(*it);
        int value = std::get<1>(*it);

        result += std::to_string(a.x);
        result += std::string(",");
        result += std::to_string(a.y);
        result += std::string(",");
        result += std::to_string(a.z);
        result += std::string(",");
        result += std::to_string(value);
        result += std::string(",");
        if(value >=0) result += source.map(value);
        result += std::string("\r\n");
    }

    return result;
}
