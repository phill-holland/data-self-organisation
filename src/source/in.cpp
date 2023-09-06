#include "in.h"
#include <iostream>

std::mt19937_64 organisation::in::generator(std::random_device{}());

void organisation::in::set(int in, int out, gate value)
{
    if(gates.find(in) == gates.end()) 
    {
        organisation::gates g;
        g.set(out, value);
        gates[in] = g;
    }
    else
    {
        gates[in].set(out, value);
    }
}

organisation::gate organisation::in::get(int in, int out)
{
    gate result;

    if(gates.find(in) != gates.end())
    {
        return gates[in].get(out);
    }

    return result;
}

std::vector<organisation::vector> organisation::in::outputs(int index)
{
    if(gates.find(index) == gates.end()) return std::vector<organisation::vector>();
        else return gates[index].get();            
}

void organisation::in::generate()
{
    clear();

    int in = (std::uniform_int_distribution<int>{0, gates::IN})(generator);     

    for(int i = 0; i < in; ++i)
    {
        int k = (std::uniform_int_distribution<int>{0, 26})(generator);         
        int out = (std::uniform_int_distribution<int>{0, gates::OUT})(generator);     
        
        gates[k].clear();

        for(int j = 0; j < out; ++j)
        {
            int magnitude = (std::uniform_int_distribution<int>{1, gate::MAGNITUDE})(generator);
            int m = (std::uniform_int_distribution<int>{0, 26})(generator);         
            
            set(k, m, gate(magnitude));
        }
    }
}

void organisation::in::mutate()
{
    int k = (std::uniform_int_distribution<int>{0, 26})(generator); 
    int m = (std::uniform_int_distribution<int>{0, 26})(generator); 

    if(get(k,m).magnitude == -1)
    {
        int magnitude = (std::uniform_int_distribution<int>{1, gate::MAGNITUDE})(generator);
        set(k,m,gate(magnitude));
    }
    else set(k,m,gate());
}

bool organisation::in::validate(int &in)
{
    in = 0;

    for(auto &it: gates)
    {
        int out = 0;
        if(!it.second.validate(out)) 
            return false;

        if(out > 0) ++in;
    }

    return true;
}

bool organisation::in::equals(const in &source)
{
    if(gates.size() != source.gates.size()) return false;

    for(auto &it: gates)
    {
        if(source.gates.find(it.first) == source.gates.end()) return false;

        auto temp = source.gates.at(it.first);
        if(!temp.equals(it.second)) return false;                
    }

    return true;
}

void organisation::in::copy(const in &source)
{
    clear();
    gates = source.gates;
    /*
    for(auto &it: source.gates)
    {
        gates[it.first] = it.second;
    }*/
    //gates.insert(source.gates.begin(), source.gates.end());
}
