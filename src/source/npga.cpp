#include "npga.h"
#include "general.h"
#include <iostream>
#include <fcntl.h>
#include <future>
#include <algorithm>
#include <signal.h>
#include <chrono>

std::mt19937_64 organisation::populations::npga::generator(std::random_device{}());

void organisation::populations::npga::reset(parameters &params)
{
    init = false; cleanup();

    settings = params;

    dimensions = 0;
    std::vector<std::vector<std::string>> d;
    for(std::vector<std::string>::iterator it = settings.expected.begin(); it != settings.expected.end(); ++it)
    {
        std::vector<std::string> t = split(*it);
        dimensions += (t.size() * 2) + 1;
    }

    frontA = new organisation::parallel::front(*settings.dev, dimensions, settings.clients);
    if (frontA == NULL) return;
    if (!frontA->initalised()) return;

    frontB = new organisation::parallel::front(*settings.dev, dimensions, settings.clients);
    if (frontB == NULL) return;
    if (!frontB->initalised()) return;

    schemas = new organisation::schema*[settings.size];
    if(schemas == NULL) return;
    for(int i = 0; i < settings.size; ++i) schemas[i] = NULL;
    for(int i = 0; i < settings.size; ++i)
    {
        schemas[i] = new organisation::schema(settings.params.width, settings.params.height, settings.params.depth);
        if(schemas[i] == NULL) return;
        if(!schemas[i]->initalised()) return;
    }

    // ***

    intermediateA = new organisation::schema*[settings.clients];
    if (intermediateA == NULL) return;
    for(int i = 0; i < settings.clients; ++i) intermediateA[i] = NULL;
    for(int i = 0; i < settings.clients; ++i)
    {
        intermediateA[i] = new organisation::schema(settings.params.width, settings.params.height, settings.params.depth);
        if(intermediateA[i] == NULL) return;
        if(!intermediateA[i]->initalised()) return;
    }

    intermediateB = new organisation::schema*[settings.clients];
    if (intermediateB == NULL) return;
    for(int i = 0; i < settings.clients; ++i) intermediateB[i] = NULL;
    for(int i = 0; i < settings.clients; ++i)
    {
        intermediateB[i] = new organisation::schema(settings.params.width, settings.params.height, settings.params.depth);
        if(intermediateB[i] == NULL) return;
        if(!intermediateB[i]->initalised()) return;
    }

    intermediateC = new organisation::schema*[settings.clients];
    if (intermediateC == NULL) return;
    for(int i = 0; i < settings.clients; ++i) intermediateC[i] = NULL;
    for(int i = 0; i < settings.clients; ++i)
    {
        intermediateC[i] = new organisation::schema(settings.params.width, settings.params.height, settings.params.depth);
        if(intermediateC[i] == NULL) return;
        if(!intermediateC[i]->initalised()) return;
    }

    distancesA = new float[settings.clients];
    if(distancesA == NULL) return;

    distancesB = new float[settings.clients];
    if(distancesB == NULL) return;

    // ***

    programs = new parallel::program(*settings.dev, settings.params, settings.clients);
    if (programs == NULL) return;
    if (!programs->initalised()) return;

    init = true;
}

void organisation::populations::npga::clear()
{
    for(int i = 0; i < settings.size; ++i)
    {
        schemas[i]->clear();
    }
}

void organisation::populations::npga::generate()
{
    for(int i = 0; i < settings.size; ++i)
    {
        schemas[i]->generate(settings.mappings);
    }
}

organisation::schema organisation::populations::npga::go(std::vector<std::string> expected, int &count, int iterations)
{    
    float highest = 0.0f;
    bool finished = false;
    count = 0;

    schema res(settings.params.width, settings.params.height, settings.params.depth);

    organisation::schema **set = intermediateC, **run = intermediateA, **get = intermediateB;

    region rset = { 0, (settings.size / 2) - 1 };
    region rget = { (settings.size / 2), settings.size - 1 };

    pull(intermediateA, rset, frontA, distancesA);

    do
    {
        auto r1 = std::async(&organisation::populations::npga::push, this, set, rset);
        auto r2 = std::async(&organisation::populations::npga::pull, this, get, rget, frontA, distancesA);
        auto r3 = std::async(&organisation::populations::npga::execute, this, run, expected);

        r1.wait();
        r2.wait();
        r3.wait();

        organisation::populations::results result = r3.get();

        if(result.best > highest)
        {
            res.copy(*run[result.index]);
            highest = result.best;
        }

        if(result.best >= 0.9999f) finished = true;    
        
        std::cout << "Generation (" << count << ") Best=" << result.best;
        std::cout << " Highest=" << highest;
        std::cout << " Avg=" << result.average;
        std::cout << "\r\n";

        if((iterations > 0)&&(count > iterations)) finished = true;

        organisation::schema **t1 = set;
        set = run;
        run = get;
        get = t1;

        region t2 = rset;
        rset = rget;
        rget = t2;

        ++count;

    } while(!finished);

    return res;
}

organisation::populations::results organisation::populations::npga::execute(organisation::schema **buffer, std::vector<std::string> expected)
{  
    auto combine = [](std::vector<std::string> expected, std::vector<std::string> output)
    {
        std::vector<std::tuple<std::string,std::string>> result(expected.size());

        int i = 0;
        for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
        {            
            std::tuple<std::string,std::string> temp(*it,output.at(i));
            result[i] = temp;
            ++i;
        }

        return result;
    };

    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();   

    int x1 = settings.params.width / 2;
    int y1 = settings.params.height / 2;
    int z1 = settings.params.depth / 2;

    organisation::vector w {0,1,0};

    std::vector<sycl::float4> positions;

    int j = 0;
    for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
    {
        positions.push_back( { x1 + j, y1, z1, w.encode() } );
        ++j;
    }

    programs->clear(settings.q);
    programs->copy(buffer, settings.clients, settings.q);
    programs->set(positions, settings.q);
    programs->run(settings.q);

    std::vector<organisation::parallel::output> values = programs->get(settings.mappings, settings.q);
        
    results result;
    std::vector<std::string> current;

    int i = 0;
    std::vector<organisation::parallel::output>::iterator it;    
    for(i = 0, it = values.begin(); it != values.end(); it++, i++)    
    {
        buffer[i]->compute(combine(expected, it->values));

        float score = buffer[i]->sum();
        if(score > result.best)
        {
            result.best = score;
            result.index = i;
            current = it->values;
        }
        
        result.average += score;
    }


    std::cout << "result.index [" << result.index << "] " << result.best << "\r\n";
    for(std::vector<std::string>::iterator it = current.begin(); it != current.end(); ++it)
    {
        std::string temp = *it;
        if(temp.size() > 80)
        {
            temp.resize(80);
            temp += "...";
        }

        std::cout << temp << "\r\n";
    }
    
    result.average /= (float)values.size();

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "execute " << time_span.count() << "\r\n";    

    return result;
}

bool organisation::populations::npga::get(schema &destination, region r, organisation::parallel::front *front, float *distances)
{
    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)settings.size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, settings.size - 1})(generator);

    if(((float)t) <= mutation) 
    {
        schema *s1 = best(r, front, distances);
        if(s1 == NULL) return false;

        destination.copy(*s1);
        destination.mutate(settings.mappings);            
    }
    else
    {
        schema *s1 = best(r, front, distances);
        if(s1 == NULL) return false;
        schema *s2 = best(r, front, distances);
        if(s2 == NULL) return false;
                     
        s1->cross(&destination, s2);
    }

    return true;
}

bool organisation::populations::npga::set(schema &source, region r)
{    
    schema *destination = worst(r);
    if(destination == NULL) return false;

    destination->copy(source);

    return true;
}

organisation::schema *organisation::populations::npga::best(region r, organisation::parallel::front *front, float *distances)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    int competition;

    int best = rand(generator);    
	float score = schemas[best]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);

        schemas[best]->get(temp1, minimum, maximum);
        schemas[competition]->get(temp2, minimum, maximum);

		float t2 = schemas[competition]->sum();

        if(temp2.dominates(temp1))
        {
            best = competition;
            score = t2;
        }         
	}

    return schemas[best];      
}

organisation::schema *organisation::populations::npga::worst(region r)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    int competition;
    int worst = rand(generator);

	float score = schemas[worst]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);

        schemas[worst]->get(temp1, minimum, maximum);
        schemas[competition]->get(temp2, minimum, maximum);

		float t2 = schemas[competition]->sum();

        if(temp1.dominates(temp2))
        {
            worst = competition;             
            score = t2;
        }
	}

    return schemas[worst];
}

void organisation::populations::npga::pull(organisation::schema **buffer, region r, organisation::parallel::front *destination, float *distances)
{
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();   

    pick(r, destination, distances);

    for(int i = 0; i < settings.clients; ++i)
    {
        get(*buffer[i], r, destination, distances);
    }    

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "pull " << time_span.count() << "\r\n";    
}

void organisation::populations::npga::push(organisation::schema **buffer, region r)
{
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();
        
    for(int i = 0; i < settings.clients; ++i)
    {
        set(*buffer[i], r);
    }

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "push " << time_span.count() << "\r\n";    
}


void organisation::populations::npga::pick(region r, organisation::parallel::front *destination, float *distances)
{
    if(settings.fronts != (settings.size / 2)) return;

    int index = 0;
    for(int i = r.start; i <= r.end; ++i)
    {         
        destination->set(schemas[i], index++);
    }

    destination->run(settings.q);
    crowded(r, distances);

    /*
    std::uniform_int_distribution<int> rand{ r.start, r.end };

    if(settings.fronts != (settings.size / 2))
    {
        for(int i = 0; i < settings.fronts; ++i)
        {
            int index = rand(generator);
            destination->set(schemas->data[index], i);
        }
    }
    else
    {
        int index = 0;
        for(int i = r.start; i <= r.end; ++i)
        {         
            destination->set(schemas->data[i], index++);
        }
    }

    destination->extra(settings.q);
    */
}

void organisation::populations::npga::sort(region r, int dimension)
{    
    auto compare = [&,dimension](schema *a, schema *b) 
	{
        float t1 = a->get(dimension);
        float t2 = b->get(dimension);

        return t1 < t2;
    };

    std::sort(schemas + r.start, schemas + r.end, compare);
}

void organisation::populations::npga::crowded(region r, float *distances)
{
    for(int i = 0; i < settings.clients; ++i)   
    {
        distances[i] = 0.0f;
    }

    for(int d = 0; d < dimensions; ++d)
    {
        sort(r, d);

        distances[0] = std::numeric_limits<float>::infinity();
        distances[settings.clients - 1] = std::numeric_limits<float>::infinity();

        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();

        for(int j = r.start; j <= r.end; ++j)
        {
            float temp = schemas[j]->get(d);

            if(temp < min) min = temp;
            if(temp > max) max = temp;
        }

        for(int j = r.start + 1; j <= r.end - 1; ++j)
        {
            distances[j] = distances[j] + (schemas[j + 1]->get(d) - schemas[j - 1]->get(d)) / (max - min);
        }
    }
}

void organisation::populations::npga::makeNull() 
{ 
    frontA = NULL;
    frontB = NULL;
    schemas = NULL;
    intermediateA = NULL;
    intermediateB = NULL;
    intermediateC = NULL;
    distancesA = NULL;
    distancesB = NULL;
    programs = NULL;
}

void organisation::populations::npga::cleanup() 
{ 
    if(programs != NULL) delete programs;
    
    if(distancesB != NULL) delete[] distancesB;
    if(distancesA != NULL) delete[] distancesA;

    if(intermediateC != NULL)
    {
        for(int i = settings.clients - 1; i >= 0; --i)
        {
            if(intermediateC[i] != NULL) delete intermediateC[i];
        }
        delete[] intermediateC;
    }
 
    if(intermediateB != NULL)
    {
        for(int i = settings.clients - 1; i >= 0; --i)
        {
            if(intermediateB[i] != NULL) delete intermediateB[i];
        }
        delete[] intermediateB;
    }

   if(intermediateA != NULL)
    {
        for(int i = settings.clients - 1; i >= 0; --i)
        {
            if(intermediateA[i] != NULL) delete intermediateA[i];
        }
        delete[] intermediateA;
    }
 
    if(schemas != NULL) 
    {
        for(int i = settings.size - 1; i >= 0; --i)
        {
            if(schemas[i] != NULL) delete schemas[i];
        }
        delete[] schemas;
    }

    if(frontB != NULL) delete frontB;
    if(frontA != NULL) delete frontA;
}