#include "schemas.h"
#include <iostream>
#include <functional>
#include <limits>

void organisation::schemas::reset(int width, int height, int depth, int size)
{
    init = false; cleanup();
    this->length = size;
    
    data.reserve(size);
    distances.reserve(size);

    //data = new organisation::schema*[size];
    //if(data == NULL) return;

    /*for(std::vector<schema*>::iterator it = data.begin(); it < data.end(); ++it)
    {
        *it = NULL;
    }*/
    for(int i = 0; i < size; ++i) { data[i] = NULL; }

    for(int i = 0; i < size; ++i) 
    //for(std::vector<schema*>::iterator it = data.begin(); it < data.end(); ++it)
    { 
        data[i] = new organisation::schema(width, height, depth);
        if(data[i] == NULL) return;

       // *it = new organisation::schema(width, height, depth);
       // if(*it == NULL) return;
    }

    //locks = new std::atomic<int>[size];
    //if(locks == NULL) return;

    //for(int i = 0; i < size; ++i) { locks[i] = 0; }

    init = true;
}

bool organisation::schemas::clear()
{
    for(int i = 0; i < length; ++i)
    //for(std::vector<schema*>::iterator it = data.begin(); it < data.end(); ++it)
    {
        //(*it)->clear();
        data[i]->clear();
    }

    return true;
}
/*
bool organisation::schemas::clear()
{
    for(int i = 0; i < length; ++i)
    {
         if(locks[i] == 1) return false;

        int a = locks[i].load();
        int b = 1;
        
        if (locks[i].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
        {
            data[i]->clear();
            locks[i] = 0;
        }
    }

    return true;
}
*/
/*
organisation::schema *organisation::schemas::lock(int index)
{
    if((index < 0)||(index >= length)) return NULL;
    if(locks[index] == 1) return NULL;

    organisation::schema *result = NULL;

    int a = locks[index].load();
    int b = 1;
    
    if (locks[index].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
    {
        result = data[index];
    }

    return result;
}

bool organisation::schemas::unlock(int index)
{
    if((index < 0)||(index >= length)) return false;
    if(locks[index] == 0) return true;

    int a = locks[index].load();
    int b = 0;
    
    if (locks[index].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
    {
        return true;
    }

    return false;
}
*/

/*
bool organisation::schemas::get(schema &destination, int index)
{
    if((index < 0)||(index >= length)) return false;
    if(locks[index] == 1) return false;

    int a = locks[index].load();
    int b = 1;
    
    if (locks[index].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
    {
        //std::cout << "get " << destination.sum() << "\r\n";
        destination.copy(*data[index]);
        locks[index] = 0;
    }

    return true;

    //destination.copy(*data[index]);
    //return true;
}

bool organisation::schemas::set(schema &source, int index)
{
    if((index < 0)||(index >= length)) return false;
  
    if(locks[index] == 1) return false;

    int a = locks[index].load();
    int b = 1;
    
    if (locks[index].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
    {
        //std::cout << "set " << source.sum() << "\r\n";
        data[index]->copy(source);
        locks[index] = 0;
    }
    
    return true;

    //data[index]->copy(source);
    //return true;
}

bool organisation::schemas::generate(organisation::data &source)
{
    for(int i = 0; i < length; ++i)
    {
         if(locks[i] == 1) return false;

        int a = locks[i].load();
        int b = 1;
        
        if (locks[i].compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
        {
            data[i]->generate(source);
            locks[i] = 0;
        }
    }

    return true;
}
*/

bool organisation::schemas::generate(organisation::data &source)
{
    for(int i = 0; i < length; ++i)
    //for(std::vector<schema*>::iterator it = data.begin(); it < data.end(); ++it)
    {
        //(*it)->generate(source);
        data[i]->generate(source);
    }

    return true;
}

void organisation::schemas::sort(int dimension)
{    
    auto compare = [&,dimension](schema *a, schema *b) 
	{
        //float t1 = a->get()[dimension];
        //float t2 = b->get()[dimension];

        float t1 = a->get(dimension);
        float t2 = b->get(dimension);

        return t1 < t2;
    };

    std::sort(data.begin(), data.end(), compare);
}

void organisation::schemas::crowded()
{
    const int dimensions = 13;

    for(int i = 0; i < length; ++i)   
    {
        distances[i] = 0.0f;
    }

    for(int d = 0; d < dimensions; ++d)
    {
        sort(d);

        distances[0] = std::numeric_limits<float>::infinity();
        distances[length - 1] = std::numeric_limits<float>::infinity();

        float min = 0.0f;
        float max = 0.0f;

        for(int j = 0; j < length; ++j)
        {
            float temp = data[j]->get(d);

            if(temp < min) min = temp;
            if(temp > max) max = temp;
        }

        for(int j = 1; j < length - 1; ++j)
        {
            distances[j] = distances[j] + (data[j + 1]->get(d) - data[j - 1]->get(d)) / (max - min);
        }
    }
}

void organisation::schemas::makeNull() 
{ 
    //data = NULL;
    //locks = NULL;
}

void organisation::schemas::cleanup() 
{ 
    //if(locks != NULL) delete[] locks;
    /*
    if(data != NULL)
    {
        for(int i = length - 1; i >= 0; i--)
        {
            if(data[i] != NULL) delete data[i];
        }

        delete[] data;
    }
    */
}