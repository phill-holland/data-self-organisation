#include "population.h"
#include "general.h"
#include <iostream>
#include <fcntl.h>
#include <future>
#include <algorithm>
#include <signal.h>
#include <chrono>
#include <valgrind/callgrind.h>

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

    frontA = new organisation::parallel::front(*settings.dev, dimensions, settings.fronts);
    if (frontA == NULL) return;
    if (!frontA->initalised()) return;

    frontB = new organisation::parallel::front(*settings.dev, dimensions, settings.fronts);
    if (frontB == NULL) return;
    if (!frontB->initalised()) return;

/*
	approximationA = new dominance::kdtree::kdtree(dimensions, 50000);
	if (approximationA == NULL) return;
	if (!approximationA->initalised()) return;

    approximationB = new dominance::kdtree::kdtree(dimensions, 50000);
	if (approximationB == NULL) return;
	if (!approximationB->initalised()) return;
*/
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

    //dominance::kdtree::kdtree *tset = approximationA;
    //dominance::kdtree::kdtree *tget = approximationB;
    organisation::parallel::front *tset = frontA;
    organisation::parallel::front *tget = frontB;

    pull(intermediateA, rset, tset);

    do
    {
        auto r1 = std::async(&organisation::populations::population::push, this, set, rset, tset);
        auto r2 = std::async(&organisation::populations::population::pull, this, get, rget, tget);
        auto r3 = std::async(&organisation::populations::population::execute, this, run, expected);

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

        organisation::parallel::front *t3 = tset;
        tset = tget;
        tget = t3;

        ++count;

    } while(!finished);

    return res;
}

organisation::populations::results organisation::populations::population::execute(organisation::schema **buffer, std::vector<std::string> expected)
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

bool organisation::populations::population::get(schema &destination, region r, organisation::parallel::front *front)
{
    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)settings.size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, settings.size - 1})(generator);

    if(((float)t) <= mutation) 
    {
        schema *s1 = best(r, front);
        if(s1 == NULL) return false;

        destination.copy(*s1);
        destination.mutate(settings.mappings);            
    }
    else
    {
        schema *s1 = best(r, front);
        if(s1 == NULL) return false;
        schema *s2 = best(r, front);
        if(s2 == NULL) return false;
                     
        s1->cross(&destination, s2);
    }

    return true;
}

bool organisation::populations::population::set(schema &source, region r, organisation::parallel::front *front)
{    
    schema *destination = worst(r, front);
    if(destination == NULL) return false;

/*
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
		if(tree->exists(temp2))
		{
			tree->remove(temp1);
		}
	}
	if(!temp2.issame(minimum)) 
    {
        tree->insert(&temp2);
        result = true;
    }
*/
    destination->copy(source);

    return true;
	//return result;
}

organisation::schema *organisation::populations::population::best(region r, organisation::parallel::front *front)
{
    const int escape = 10;

    std::uniform_int_distribution<int> rand{ 0, settings.fronts - 1 };

    int counter = 0;
    int client = 0;
    bool is_front = false;
    do
    {
        client = rand(generator);
        is_front = front->is_front(client);
        ++counter;
    }while((!is_front)&&(counter < escape));

    return front->get(client);
    /*
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };

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

		if(tree->exists(temp1))
		{
			if(tree->inside(temp1, &origin, &temp2))
			{
				score = t2;
			}
			else
			{
				best = competition;
				score = t2;
			}
            //std::cout << "goign t\r\n";
		}
        else if(t2 > score)
		{
			best = competition;
			score = t2;
		}
	}

    return schemas->data[best];
    */
}

organisation::schema *organisation::populations::population::worst(region r, organisation::parallel::front *front)
{
    const int escape = 10;

    std::uniform_int_distribution<int> rand{ 0, settings.fronts - 1 };

    int counter = 0;
    int client = 0;
    bool is_front = false;
    do
    {
        client = rand(generator);
        is_front = front->is_front(client);
        ++counter;
    }while((is_front)&&(counter < escape));

    return front->get(client);
    /*
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

		if(tree->exists(temp2))
		{
			if(tree->inside(temp2, &origin, &temp1))
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
    */
}

void organisation::populations::population::pull(organisation::schema **buffer, region r, organisation::parallel::front *front)
{
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();   

    pick(r, front);
    //front->run(settings.q);

    //CALLGRIND_START_INSTRUMENTATION;
    //CALLGRIND_TOGGLE_COLLECT;
    for(int i = 0; i < settings.clients; ++i)
    {
        get(*buffer[i], r, front);
    }    
    //CALLGRIND_TOGGLE_COLLECT;
    //CALLGRIND_STOP_INSTRUMENTATION;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "pull " << time_span.count() << "\r\n";    
}

void organisation::populations::population::push(organisation::schema **buffer, region r, organisation::parallel::front *front)
{
    std::chrono::high_resolution_clock::time_point previous = std::chrono::high_resolution_clock::now();

    pick(r, front);
    
    for(int i = 0; i < settings.clients; ++i)
    {
        set(*buffer[i], r, front);
    }
/*
    tree->clear();

    int length = r.end - r.start;
    if(settings.clients < length) length = settings.clients;

    dominance::kdtree::kdpoint temp1(dimensions);
    dominance::kdtree::kdpoint temp2(dimensions);

    temp2.set(0L);

    for(int i = 0; i < length; ++i)
    {
        schemas->data[i + r.start]->copy(*buffer[i]);
        
        temp1.set(0L);
    
        buffer[i]->get(temp1, minimum, maximum);

        if(!temp1.equals(temp2))
        {
            tree->insert(&temp1);
        }
        //set(*buffer[i], r);
    } 
*/
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - previous);   
    std::cout << "push " << time_span.count() << "\r\n";    
}

void organisation::populations::population::pick(region r, organisation::parallel::front *destination)
{
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
        for(int i = r.start; i != r.end; ++i)
        {         
            destination->set(schemas->data[i], i);
        }
    }

    destination->run(settings.q);
}

void organisation::populations::population::makeNull() 
{ 
    //approximationA = NULL;
    //approximationB = NULL;
    frontA = NULL;
    frontB = NULL;
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

    if(frontB != NULL) delete frontB;
    if(frontA != NULL) delete frontA;
//    if(approximationB != NULL) delete approximationB;    
//    if(approximationA != NULL) delete approximationA;    
}