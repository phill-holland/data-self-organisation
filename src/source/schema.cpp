#include "schema.h"
#include <iostream>
#include <tuple>

std::mt19937_64 organisation::schema::generator(std::random_device{}());

void organisation::schema::reset()
{
    init = false; cleanup();

//scores.resize(5);
    //epochs = lengths.size();

/*
    scores = new score*[epochs];
    if(scores == NULL) return;

    for(int i = 0; i < epochs; ++i) scores[i] = NULL;

    
    int counter = 0;
    for(std::vector<int>::iterator it = lengths.begin(); it != lengths.end(); ++it)
    {
        scores[counter] = new score();
        if(scores[counter] == NULL) return;

        ++counter;
    }
*/
    init = true;
}

void organisation::schema::clear()
{
    prog.clear();
    scores.clear();
    //for(int i = 0; i < epochs; ++i) scores[i]->clear();
}

void organisation::schema::generate(data &source)
{
    prog.clear();
    prog.generate(source);
}

bool organisation::schema::get(dominance::kdtree::kdpoint &destination, int minimum, int maximum)
{
    int counter = 0;
    //for(int i = 0; i < scores.size(); ++i)
    //for(std::vector<score>::iterator it = scores.begin(); it < scores.end(); ++it)
    for(auto &it: scores)
    {     
        for(int j = 0; j < it.second.size(); ++j)
        //for(auto &jt:it.second)
        {
            float temp = it.second.get(j);
            temp = (temp * ((float)maximum - minimum)) + minimum;
            if(!destination.set((long)temp, counter)) return false;

            ++counter;
        }        

#warning prog count!
        //destination.set(prog.count(), counter++);
    }

    return true;
}

float organisation::schema::sum()
{
    float result = 0.0f;
    if(scores.size() <= 0) 
        return 0.0f;
    //for(int i = 0; i < ep; ++i)
    //for(std::vector<score>::iterator it = scores.begin(); it < scores.end(); ++it)
    for(auto &it: scores)
    {
        result += it.second.sum();
    }

    //std::cout << "size " << scores.size() << "\r\n";
    return result / ((float)scores.size());
}

void organisation::schema::compute(std::vector<std::tuple<std::string,std::string>> values)
{
    int i = 0;
    for(std::vector<std::tuple<std::string,std::string>>::iterator it = values.begin(); it != values.end(); ++it)
    {
        scores[i].compute(std::get<0>(*it),std::get<1>(*it));
        ++i;
    }
}

void organisation::schema::mutate(data &source)
{
	prog.mutate(source);
}

void organisation::schema::cross(schema *destination, schema *value)
{   
    destination->prog.cross(prog, value->prog);
}

std::string organisation::schema::run(int epoch, std::string expected, data &source, history *destination)
{		
    //if((epoch < 0) || (epoch >= epochs)) return std::string("");
    //if(epoch < 

    std::string output = prog.run(epoch, source, destination);
    //if(epoch > scores.size()) scores.resize(epoch);
    // (expected.size() * 2) + 1
    scores[epoch].compute(expected, output);
    
    return output;
}

void organisation::schema::copy(const schema &source)
{    
    //std::cout << "copy " << this << "\r\n";
    prog.copy(source.prog);
    scores = source.scores;
    //scores.assign(source.scores.begin(), source.scores.end());
    
//std::cout << "copy end " << this << "\r\n";
    //int temp = epochs;
    //if(source.epochs < temp) temp = source.epochs;

    //for(int i = 0; i < temp; ++i)
    //{
        //scores[i]->copy(*source.scores[i]);    
    //}
}

void organisation::schema::makeNull()
{
    //scores = NULL;
}

void organisation::schema::cleanup()
{
    /*
    if(scores != NULL)
    {
        for(int i = epochs - 1; i >= 0; i--)
        {
            if(scores[i] != NULL) delete scores[i];
        }

        delete[] scores;
    }
    */
}