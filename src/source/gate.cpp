#include "gate.h"
#include <iostream>

void organisation::gates::clear() 
{ 
    write = 0;
    encoded = -1;
}

bool organisation::gates::is_empty() 
{ 
    return write > 0;
}

int organisation::gates::size() 
{ 
    return write;
}

void organisation::gates::set(int index, gate g)
{
    if((index < 0)||(index >= 27)) 
        return;            
    if(g.magnitude <= 0) 
        return;
    if(write >= OUT) 
        return;

    for(int i = 0; i < write; ++i)
    {
        if(data[i].encoded == index)
        {
//std::cout << "GATES exiting set i " << i << " index " << index << "\r\n";             
            data[i] = gate(g.magnitude, index);
            return;
        }
    }

//std::cout << "GATES NEW set write " << write << " index " << index << "\r\n";             
    data[write++] = gate(g.magnitude, index);
}

organisation::gate organisation::gates::get(int index)
{
    //if(data.find(index) != data.end())
    //    return data.at(index);

    for(int i = 0; i < write; ++i)
    {
        if(data[i].encoded == index) return data[i];
    }

    return gate();
}

std::vector<organisation::vector> organisation::gates::get()
{
    std::vector<organisation::vector> result;

    //for(auto &it: data)
    //{
        //if(!it.second.is_empty())
        //{
    for(int i = 0; i < write; ++i)
    {
            int index = data[i].encoded;//it.first;
            int magnitude = data[i].magnitude;//it.second.magnitude;

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
        //}            
    }

    return result;
}

std::vector<int> organisation::gates::pull()
{
    std::vector<int> result;
    
    for(int i = 0; i < write; ++i)
    {
        result.push_back(data[i].encoded);
    }
    /*
    for(auto &it: data)
    {
        if(!it.second.is_empty())
        {
            result.push_back(it.first);
        }
    }
    */

    return result;
}

bool organisation::gates::validate(int &count)
{
     if(write > gates::OUT) 
        return false;

    //count = 0;
    //for(auto &it: data)
    for(int i = 0; i < write; ++i)
    {
        //++count;
        if(data[i].magnitude > gate::MAGNITUDE) 
            return false; 

        if((data[i].encoded < 0)||(data[i].encoded > 27))
            return false;
    }

   
    return true;
}

bool organisation::gates::equals(const gates &source)
{
    if(write != source.write) return false;

    /*
    if(data.size() != source.data.size()) return false;

    for(auto &it: data)
    {
        if(source.data.find(it.first) == source.data.end()) return false;
        if(source.data.at(it.first).magnitude != it.second.magnitude) return false;
    }
    */

    return true;
}

void organisation::gates::copy(const gates &source)
{
    clear();

    encoded = source.encoded;
    write = source.write;
    for(int i = 0; i < source.write; ++i)
    {
        data[i] = source.data[i];
    }
    /*
    for(auto &a: source.data)
    {
        data[a.first] = gate(a.second.magnitude);
    }
    */
    //data = source.data;
}