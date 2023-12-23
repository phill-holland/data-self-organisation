#include "in.h"
#include <iostream>
#include <algorithm>

std::mt19937_64 organisation::in::generator(std::random_device{}());

void organisation::in::clear()
{
    //gates.clear();
    write = 0;
}

bool organisation::in::is_empty(int index)
{
    //if(gates.find(index) == gates.end()) return true;
 
    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [index](organisation::gates const &source) { return source.encoded == index; });

    if(result == std::begin(gates) + write) return true;

    return false;
}

void organisation::in::set(int in)
{
    if(write >= gates::IN) 
        return;
    if(in == -1) 
        return;

    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [in](organisation::gates const &source) { return source.encoded == in; });

    //if(gates.find(in) == gates.end()) 
    if(result == std::begin(gates) + write)
    {
//std::cout << "set in " << in << " write " << write << "\r\n";         
        gates[write].clear();
        gates[write++].encoded = in;

        //organisation::gates g;
        //g.encoded = in;
        //gates[write++] = g;
        //gates[in] = g;
    }    
}

void organisation::in::set(int in, int out, gate value)
{
    if(write >= gates::IN) 
        return;
    if(in == -1) 
        return;

    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [in](organisation::gates const &source) { return source.encoded == in; });

    //if(gates.find(in) == gates.end()) 
    if(result == std::begin(gates) + write)
    {
        //std::cout << "AA set in NEW " << in << " out " << out << " write " << write << "\r\n";

        gates[write].clear();
        gates[write].encoded = in;
        gates[write].set(out, value);
        ++write;    
        /*
        organisation::gates g;
        g.encoded = in;
        g.set(out, value);

        gates[in] = g;
        */
    }
    else
    {
        //std::cout << "AA set existing in " << in << " out " << out << " write " << write << "\r\n";

        result->set(out, value);        
        //gates[in].set(out, value);
    }
}

organisation::gate organisation::in::get(int in, int out)
{
    //gate result;

    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [in](organisation::gates const &source) { return source.encoded == in; });

    //if(gates.find(in) != gates.end())
    if(result != std::begin(gates) + write)
    {
        return result->get(out);
        //return gates[in].get(out);
    }

    return gate();
    //return result;
}

std::vector<organisation::vector> organisation::in::outputs(int index)
{
    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [index](organisation::gates const &source) { return source.encoded == index; });

    if(result == std::begin(gates) + write) return std::vector<organisation::vector>();
    
    return result->get();

    //if(gates.find(index) == gates.end()) return std::vector<organisation::vector>();
    //    else return gates[index].get();            
}

void organisation::in::generate()
{
    //std::cout << "generate\r\n";
    clear();

    int in = (std::uniform_int_distribution<int>{0, gates::IN})(generator);     

//in = 2;    
//in = 2;
//in = gates::IN + 5;
    for(int i = 0; i < in; ++i)
    {
        int k = (std::uniform_int_distribution<int>{0, 26})(generator);         
        int out = (std::uniform_int_distribution<int>{0, gates::OUT})(generator);     

//out = 5;
//if(i == 5) 
//clear();
//std::cout << "first K " << k << "\r\n";
        set(k);      
//#warning might need this line below!        
        //gates[k].clear();

//k = 16;

//out = gates::OUT + 5;

        for(int j = 0; j < out; ++j)
        {
            int magnitude = (std::uniform_int_distribution<int>{1, gate::MAGNITUDE})(generator);
            int m = (std::uniform_int_distribution<int>{0, 26})(generator);         
//m = 10;
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

    //for(auto &it: gates)
    for(int i = 0; i < write; ++i)
    {
        int out = 0;
        if(!gates[i].validate(out)) 
            return false;

        if(out > 0) ++in;
    }

    return true;
}

std::vector<int> organisation::in::pull()
{
    std::vector<int> result;

    for(int i = 0; i < write; ++i) result.push_back(gates[i].encoded);
/*
    for(auto &it: gates)
    {
        result.push_back(it.first);
    }
*/
    return result;
}

std::vector<int> organisation::in::pull(int in)
{
    organisation::gates *result = std::find_if(std::begin(gates),std::begin(gates) + write,
    [in](organisation::gates const &source) { return source.encoded == in; });

    if(result != std::begin(gates) + write)
    {
        return result->pull();
    }
    /*
    if(gates.find(in) != gates.end())
    {
        return gates.at(in).pull();
    }
    */
    return std::vector<int>();
}
        
bool organisation::in::equals(const in &source)
{
    /*
    if(gates.size() != source.gates.size()) return false;

    for(auto &it: gates)
    {
        if(source.gates.find(it.first) == source.gates.end()) return false;

        auto temp = source.gates.at(it.first);
        if(!temp.equals(it.second)) return false;                
    }
    */
    return true;
}

void organisation::in::copy(const in &source)
{
    clear();

    write = source.write;    
    for(int i = 0; i < source.write; ++i)
    {
        gates[i].copy(source.gates[i]);
    }
    //gates = source.gates;
    /*
    for(auto &a: source.gates)
    {
        gates[a.first].copy(a.second);
    }
    */
}
