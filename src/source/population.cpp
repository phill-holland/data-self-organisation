#include "population.h"
#include "general.h"
#include <iostream>
#include <fcntl.h>
#include <future>
#include <algorithm>
#include <signal.h>
#include <chrono>

std::mt19937_64 organisation::populations::population::generator(std::random_device{}());

void organisation::populations::population::reset(parameters &params)
{
    init = false; cleanup();

    settings = params;

    dimensions = 0;
    std::vector<std::vector<std::string>> d;
    for(std::vector<std::string>::iterator it = settings.expected.begin(); it != settings.expected.end(); ++it)
    {
        std::vector<std::string> t = split(*it);
        //lengths.push_back((t.size() * 2) + 1);
        dimensions += (t.size() * 2) + 1;
    }

	approximation = new dominance::kdtree::kdtree(dimensions, 10000);
	if (approximation == NULL) return;
	if (!approximation->initalised()) return;

    schemas = new organisation::schemas(settings.params.width, settings.params.height, settings.params.depth, settings.size);
    if (schemas == NULL) return;
    if (!schemas->initalised()) return;

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

    // ***

    programs = new parallel::program(*settings.dev, settings.params, settings.clients);
    if (programs == NULL) return;
    if (!programs->initalised()) return;

    init = true;
}

void organisation::populations::population::clear()
{
    schemas->clear();
}

void organisation::populations::population::generate()
{
    schemas->generate(settings.mappings);
}

organisation::schema organisation::populations::population::go(std::vector<std::string> expected, int &count, int iterations)
{    
    float highest = 0.0f;
    bool finished = false;
    count = 0;

    schema res(settings.params.width, settings.params.height, settings.params.depth);

    organisation::schema **set = intermediateC, **run = intermediateA, **get = intermediateB;

    region rset = { 0, (settings.size / 2) - 1 };
    region rget = { (settings.size / 2), settings.size - 1 };

    pull(intermediateA, rset);

    do
    {
        /*
        if(set == intermediateA) std::cout << "set A\r\n";
        else if(set == intermediateB) std::cout << "set B\r\n";
        else if(set == intermediateC) std::cout << "set C\r\n";
        
        std::cout << "rset " << rset.start << " " << rset.end << "\r\n";

        if(run == intermediateA) std::cout << "run A\r\n";
        else if(run == intermediateB) std::cout << "run B\r\n";
        else if(run == intermediateC) std::cout << "run C\r\n";

        if(get == intermediateA) std::cout << "get A\r\n";
        else if(get == intermediateB) std::cout << "get B\r\n";
        else if(get == intermediateC) std::cout << "get C\r\n";

        std::cout << "rget " << rget.start << " " << rget.end << "\r\n";
    */
        auto r1 = std::async(&organisation::populations::population::push, this, set, rset);
        auto r2 = std::async(&organisation::populations::population::pull, this, get, rget);
        auto r3 = std::async(&organisation::populations::population::execute, this, run, expected[0]);

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

organisation::populations::results organisation::populations::population::execute(organisation::schema **buffer, std::string expected)
{  
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();   

    int x1 = settings.params.width / 2;
    int y1 = settings.params.height / 2;
    int z1 = settings.params.depth / 2;

    organisation::vector w {0,1,0};

    std::vector<sycl::float4> positions;

    for(int i = 0; i < settings.clients; ++i)
    {
        positions.push_back({x1,y1,z1,w.encode()});
    }

    programs->clear(settings.q);
    programs->copy(buffer, settings.clients, settings.q);
    programs->set(positions, settings.q);
    programs->run(settings.q);

    std::vector<organisation::parallel::output> values = programs->get(settings.q);
    
    int i = 0;
    std::vector<organisation::parallel::output>::iterator it;
    
    results result;
    
    for(i = 0, it = values.begin(); it != values.end(); ++it, ++i)    
    {
        std::string output = settings.mappings.get(it->values);
        buffer[i]->scores[0].compute(expected, output);
        float score = buffer[i]->sum();
        if(score > result.best)
        {
            result.best = score;
            result.index = i;
        }
        result.average += score;
    }

    result.average /= (float)values.size();

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "execute " << time_span.count() << "\r\n";    

    return result;
}

bool organisation::populations::population::get(schema &destination, region r)
{
    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)settings.size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, settings.size - 1})(generator);

    if(((float)t) <= mutation) 
    {
        schema *s1 = best(r);
        if(s1 == NULL) return false;

        destination.copy(*s1);
        destination.mutate(settings.mappings);            
    }
    else
    {
        schema *s1 = best(r);
        if(s1 == NULL) return false;
        schema *s2 = best(r);
        if(s2 == NULL) return false;
                     
        s1->cross(&destination, s2);
    }

    return true;
}

bool organisation::populations::population::set(schema &source, region r)//int index)
{    
    schema *destination = worst(r);
    if(destination == NULL) return false;

	if(source.sum() < destination->sum()) return false;

    std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
    bool result = false;
   
	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);

	temp1.set(0L);
	temp2.set(0L);

    destination->get(temp1, minimum, maximum);
    source.get(temp2, minimum, maximum);

	if(!temp1.issame(minimum)) 
	{
		if(approximation->exists(temp2))
		{
			approximation->remove(temp1);
		}
	}
	if(!temp2.issame(minimum)) 
    {
        approximation->insert(&temp2);
        result = true;
    }

    destination->copy(source);

	return result;
}

organisation::schema *organisation::populations::population::best(region r)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };//size - 1 };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    int competition;

    int best = rand(generator);    
	float score = schemas->data[best]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);

        schemas->data[best]->get(temp1, minimum, maximum);
        schemas->data[competition]->get(temp2, minimum, maximum);

		float t2 = schemas->data[competition]->sum();

		if(approximation->exists(temp1))
		{
			if(approximation->inside(temp1, &origin, &temp2))
			{
				score = t2;
			}
			else
			{
				best = competition;
				score = t2;
			}
		}
        else if(t2 > score)
		{
			best = competition;
			score = t2;
		}
	}

    return schemas->data[best];
}

organisation::schema *organisation::populations::population::worst(region r)
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

	float score = schemas->data[worst]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);

        schemas->data[worst]->get(temp1, minimum, maximum);
        schemas->data[competition]->get(temp2, minimum, maximum);

		float t2 = schemas->data[competition]->sum();

		if(approximation->exists(temp2))
		{
			if(approximation->inside(temp2, &origin, &temp1))
			{
				score = t2;
			}
			else 
			{
                worst = competition;             
				score = t2;
			}
		}
		else if(t2 < score)
		{
            worst = competition;
			score = t2;
		}
	}

    return schemas->data[worst];
}

void organisation::populations::population::pull(organisation::schema **buffer, region r)
{
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();   

    for(int i = 0; i < settings.clients; ++i)
    {
        get(*buffer[i], r);
    }    

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "pull " << time_span.count() << "\r\n";    
}

void organisation::populations::population::push(organisation::schema **buffer, region r)
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

void organisation::populations::population::makeNull() 
{ 
    approximation = NULL;
    schemas = NULL;
    intermediateA = NULL;
    intermediateB = NULL;
    intermediateC = NULL;
    programs = NULL;
}

void organisation::populations::population::cleanup() 
{ 
    if(programs != NULL) delete programs;
    
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
 
    if(schemas != NULL) delete schemas;
    if(approximation != NULL) delete approximation;    
}