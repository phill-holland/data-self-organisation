#include "schemas.h"
#include <iostream>

void organisation::schemas::reset(int size)
{
    init = false; cleanup();
    this->length = size;
    
    data = new organisation::schema*[size];
    if(data == NULL) return;

    for(int i = 0; i < size; ++i) { data[i] = NULL; }

    for(int i = 0; i < size; ++i) 
    { 
        data[i] = new organisation::schema();
        if(data[i] == NULL) return;
    }

    locks = new std::atomic<int>[size];
    if(locks == NULL) return;

    for(int i = 0; i < size; ++i) { locks[i] = 0; }

    init = true;
}

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

void organisation::schemas::makeNull() 
{ 
    data = NULL;
    locks = NULL;
}

void organisation::schemas::cleanup() 
{ 
    if(locks != NULL) delete[] locks;
    
    if(data != NULL)
    {
        for(int i = length - 1; i >= 0; i--)
        {
            if(data[i] != NULL) delete data[i];
        }

        delete[] data;
    }
}