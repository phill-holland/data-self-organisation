#include "schema.h"
#include <iostream>
#include <tuple>

std::mt19937_64 organisation::schema::generator(std::random_device{}());

void organisation::schema::reset(std::vector<int> lengths)
{
    init = false; cleanup();

    epochs = lengths.size();

    scores = new score*[epochs];
    if(scores == NULL) return;

    for(int i = 0; i < epochs; ++i) scores[i] = NULL;

    int counter = 0;
    for(std::vector<int>::iterator it = lengths.begin(); it != lengths.end(); ++it)
    {
        scores[counter] = new score(*it);
        if(scores[counter] == NULL) return;

        ++counter;
    }

    init = true;
}

void organisation::schema::clear()
{
    prog.clear();
    for(int i = 0; i < epochs; ++i) scores[i]->clear();
}

void organisation::schema::generate(data &source)
{
    prog.clear();
    prog.generate(source);
}

bool organisation::schema::get(dominance::kdtree::kdpoint &destination, int minimum, int maximum)
{
    int counter = 0;
    for(int i = 0; i < epochs; ++i)
    {
        for(int j = 0; j < scores[i]->size(); ++j)
        {
            float temp = scores[i]->get(j);
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

    for(int i = 0; i < epochs; ++i)
    {
        result += scores[i]->sum();
    }

    return result / ((float)epochs);
}

void organisation::schema::mutate(data &source)
{
	prog.mutate(source);
}

void organisation::schema::cross(schema *destination, schema *value)
{   
    //threading::semaphore lock_a(token), lock_b(value->token); 
    destination->prog.cross(prog, value->prog);
}

std::string organisation::schema::run(int epoch, std::string expected, data &source, history *destination)
{		
    if((epoch < 0) || (epoch >= epochs)) return std::string("");

    std::string output = prog.run(epoch, source, destination);
    scores[epoch]->compute(expected, output);
    
    return output;
}

void organisation::schema::copy(schema &source)
{    
    //threading::semaphore lock_a(token), lock_b(source.token); 
    prog.copy(source.prog);

    int temp = epochs;
    if(source.epochs < temp) temp = source.epochs;

    for(int i = 0; i < temp; ++i)
    {
        scores[i]->copy(*source.scores[i]);    
    }
}

void organisation::schema::makeNull()
{
    scores = NULL;
}

void organisation::schema::cleanup()
{
    if(scores != NULL)
    {
        for(int i = epochs - 1; i >= 0; i--)
        {
            if(scores[i] != NULL) delete scores[i];
        }

        delete[] scores;
    }
}