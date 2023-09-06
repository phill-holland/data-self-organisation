#include "gate.h"

void organisation::gates::set(int index, gate g)
{
    if((index < 0)||(index >= 27)) return;            
    if(g.magnitude <= 0) return;

    data[index] = g;
}

organisation::gate organisation::gates::get(int index)
{
    if(data.find(index) != data.end())
        return data.at(index);

    return gate();
}

std::vector<organisation::vector> organisation::gates::get()
{
    std::vector<organisation::vector> result;

    for(auto &it: data)
    {
        if(!it.second.is_empty())
        {
            int index = it.first;
            int magnitude = it.second.magnitude;

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

bool organisation::gates::validate(int &count)
{
    count = 0;
    for(auto &it: data)
    {
        ++count;
        if(it.second.magnitude > gate::MAGNITUDE) 
            return false; 
    }

    if(count > gates::OUT) 
        return false;

    return true;
}

bool organisation::gates::equals(const gates &source)
{
    if(data.size() != source.data.size()) return false;

    for(auto &it: data)
    {
        if(source.data.find(it.first) == source.data.end()) return false;
        if(source.data.at(it.first).magnitude != it.second.magnitude) return false;
    }

    return true;
}

void organisation::gates::copy(const gates &source)
{
    clear();
    data = source.data;
    //data.insert(source.data.begin(), source.data.end());
}