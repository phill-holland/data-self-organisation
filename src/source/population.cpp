#include "population.h"
#include "general.h"
#include <iostream>
#include <fcntl.h>
#include <future>
#include <algorithm>
#include <signal.h>

std::mt19937_64 organisation::populations::population::generator(std::random_device{}());

void organisation::populations::population::reset(parameters &params)//organisation::data source, std::vector<std::string> expected, int size)
{
    init = false; cleanup();

    settings = params;
    //this->size = size;

    //mappings = source;

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
    //programs->clear();
}

void organisation::populations::population::generate()
{
    schemas->generate(settings.mappings);
}

organisation::schema organisation::populations::population::go(std::vector<std::string> expected, int &count, int iterations)
{
    schema res(settings.params.width, settings.params.height, settings.params.depth);
    float most = 0.0f;

    bool result = false;
    count = 0;

    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)settings.size) / 100.0f) * mutate_rate_in_percent;

    //schema **source = left, **destination = right;
    //generate();

    //int x1 = settings.params.width / 2;
    //int y1 = settings.params.height / 2;
    //int z1 = settings.params.depth / 2;

    //organisation::vector w {0,1,0};

    organisation::schema **set = intermediateC, **run = intermediateA, **get = intermediateB;

    region rset = { 0, (settings.size / 2) - 1 };
    region rget = { (settings.size / 2), settings.size - 1 };

    pull(intermediateA, rset);
    pull(intermediateC, rget);

    do
    {
        float total = 0.0f;        
      
      //auto result = std::async(&organisation::population::population::run, this, expected, mutation);
        auto r1 = std::async(&organisation::populations::population::pull, this, set, rset);
        auto r2 = std::async(&organisation::populations::population::push, this, get, rget);
        auto r3 = std::async(&organisation::populations::population::execute, this, run);

        r1.wait();
        r2.wait();
        r3.wait();

        organisation::schema **t1 = set;
        set = run;
        run = get;
        get = t1;

        region t2 = rset;
        rset = rget;
        rget = t2;

        // *** set
        
        //push(set, rset);

        // *** set

        // *** get

        //pull(get, rget);

        // *** get

        // *** run
/*
        std::vector<sycl::float4> positions;

        for(int i = 0; i < settings.clients; ++i)
        {
            positions.push_back({x1,y1,z1,w.encode()});
        }
    
        programs->clear(settings.q);
        programs->copy(run, settings.clients, settings.q);
        programs->set(positions, settings.q);
        programs->run(settings.q);
        std::vector<organisation::parallel::output> values = programs->get(settings.q);
        */
        // *** run

// change programs->copy to accept pointers to programs
// change schema to return pointers...??

           // if(buffer.size() > 0)
            //{
                //programs->clear(settings.q);
                //programs->copy(buffer, settings.q);
                //programs->set(positions, settings.q);
                //programs->run(settings.q);

                // get results, compute scores for schemas! (recreate new schemas for reinsertion into population?)
            //}
            // run programs (loop through expected epochs!)
            // set scores in schemas
            // push back into population

            std::vector<std::future<std::tuple<std::vector<std::string>,float>>> results;
/*
            int m = threads;
            if(generation + threads >= size) 
                m = size - generation;
            for(int i = 0; i < m; ++i)
            {
             //std::cout << "start " << intermediate[i] << "\r\n";   
                //std::cout << (i + generation) << "\r\n";
                auto result = std::async(&organisation::population::population::run, this, expected, mutation);
                results.push_back(std::move(result));
            }
*/
            

            for (std::vector<std::future<std::tuple<std::vector<std::string>,float>>>::iterator it = results.begin(); it != results.end(); ++it)
            {
                std::tuple<std::vector<std::string>,float> outputs = it->get();

                std::vector<std::string> output = std::get<0>(outputs);
                float sum = std::get<1>(outputs);
                
                total += sum;

                if(sum > most)
                {
                    //res.copy(*dest);
                    most = sum;
                }

                if(sum >= 0.9999f) result = true;    
                if(output == expected) result = true;   
                          
            }      
        //}
        
        total /= settings.size;
        
        std::cout << "Generation (" << count << ") Best=" << most;
        std::cout << " Avg=" << total;
        //std::cout << " IC=" << incoming.entries();
        //std::cout << " OC=" << outgoing.entries();
        std::cout << "\r\n";

        if((iterations > 0)&&(count > iterations)) result = true;

        ++count;

    } while(!result);

    return res;
}

void organisation::populations::population::execute(organisation::schema **buffer)
{
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
}
/*
std::tuple<std::vector<std::string>,float> organisation::population::population::run(std::vector<std::string> expected, const float mutation)
{    
    std::vector<std::string> results;

        organisation::schema destination;
        if(!get(destination)) return std::tuple<std::vector<std::string>,float>(results,0.0f);        

        int epoch = 0;
        for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
        {
            results.push_back(destination.run(epoch, *it, settings.mappings));
            ++epoch;
        }

    set(destination);

    return std::tuple<std::vector<std::string>,float>(results,destination.sum());
}
*/
/*
void organisation::population::population::back(schema **destination, schema **source, int thread)
{
    
    int block_size = 1000 / threads;

    for(int i = 0; i < block_size; ++i)
    {
        organisation::schema *b = best(source);
        int start = thread * block_size;
        int end = start + block_size;
        int w = worst(destination, start, end);

        if(destination[w]->sum() < b->sum()) 
            destination[w]->copy(*b);
    }
}
*/
//organisation::schema organisation::population::population::top()
//{
    /*
    float s = 0.0f;
    int j = 0;
    
    for(int i = 0; i < size; ++i)
    {
        if(data[i]->sum() > s)
        {
            j = i;
            s = data[i]->sum();
        }

    }

    return *data[j];
    */
   //return *left[0];
   //return organisation::schema();
//}

bool organisation::populations::population::get(schema &destination, region r)
{
    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)settings.size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, settings.size - 1})(generator);

    if(((float)t) <= mutation) 
    {
        //schema s1(settings.params.width, settings.params.height, settings.params.depth);
        //if(!best(s1)) return false;
        schema *s1 = best(r);
        if(s1 == NULL) return false;

        destination.copy(*s1);
        destination.mutate(settings.mappings);            
    }
    else
    {
        //schema s1(settings.params.width, settings.params.height, settings.params.depth);
        //schema s2(settings.params.width, settings.params.height, settings.params.depth);

        //if(!best(s1)) return false;
        //if(!best(s2)) return false;
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
    //organisation::schema destination(settings.params.width, settings.params.height, settings.params.depth);
    //int index = worst(destination);
    schema *destination = worst(r);
    if(destination == NULL) return false;

    //if(index < 0) return false;
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

    //destination[index]->copy(*source);    
    //if(!schemas->set(source, index)) return false;

    destination->copy(source);

	return result;
}

/*
organisation::schema *organisation::population::population::best(schema **source)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ 0, size - 1 };

    long competition;

    long best = rand(generator);
	float score = source[best]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);
        float t2 = source[competition]->sum();

        if(t2 > score)
        {
            best = competition;
            score = t2;
        }        
	}

	return source[best];
}

int organisation::population::population::worst(schema **source, int start, int end)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ start, end - 1 };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    int competition;

    int worst = rand(generator);
	float score = source[worst]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);
        float t2 = source[competition]->sum();

         if(t2 < score)
		{
			worst = competition;
			score = t2;
		}
	}

	return worst;
}
*/

organisation::schema *organisation::populations::population::best(region r)//organisation::schema &destination, organisation::schema &competition)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };//size - 1 };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

   //organisation::schema competition(settings.params.width, settings.params.height, settings.params.depth);
   //if(pull(destination)<0) return false;

    int competition;

    int best = rand(generator);    
	float score = schemas->data[best]->sum();//destination.sum();//source[best]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);
        //if(pull(competition)<0) return false;

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

    //return true;
	//return source[best];
    return schemas->data[best];
}

/*
int organisation::populations::population::pull(organisation::schema &destination)
{
    std::uniform_int_distribution<int> rand{ 0, settings.size - 1 };
    int escape = 0, index = rand(generator);
    while((!schemas->get(destination, index))&&(++escape<15)) { index = rand(generator); }
    if(escape>=15) return -1;
    return index;
}
*/

organisation::schema *organisation::populations::population::worst(region r)
{
    const int samples = 10;

	std::uniform_int_distribution<int> rand{ r.start, r.end };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    //int result = -1;

    int competition;
    //int temp;
    int worst = rand(generator);
    //organisation::schema competition(settings.params.width, settings.params.height, settings.params.depth);
    //result = pull(destination);
    //if(result < 0) return -1;

	float score = schemas->data[worst]->sum();//destination.sum();//data[worst]->sum();

	for (int i = 0; i < samples; ++i)
	{
		competition = rand(generator);
        //temp = pull(destination);
        //if(temp < 0) return -1;

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
				//destination = competition;
                worst = competition;
                //result = temp;
				score = t2;
			}
		}
		else if(t2 < score)
		{
			//destination = competition;
            //result = temp;
            worst = competition;
			score = t2;
		}
	}

    return schemas->data[worst];
    //return result;
    //return true;
	//return worst;
}

void organisation::populations::population::pull(organisation::schema **buffer, region r)
{
    for(int i = 0; i < settings.clients; ++i)
    {
        get(*buffer[i], r);
    }    
}

void organisation::populations::population::push(organisation::schema **buffer, region r)
{
    for(int i = 0; i < settings.clients; ++i)
    {
        set(*buffer[i], r);
    }    
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
            if(intermediateC[i] != NULL) delete intermediateC;
        }
        delete[] intermediateC;
    }
 
    if(intermediateB != NULL)
    {
        for(int i = settings.clients - 1; i >= 0; --i)
        {
            if(intermediateB[i] != NULL) delete intermediateB;
        }
        delete[] intermediateB;
    }

   if(intermediateA != NULL)
    {
        for(int i = settings.clients - 1; i >= 0; --i)
        {
            if(intermediateA[i] != NULL) delete intermediateA;
        }
        delete[] intermediateA;
    }
 
    if(schemas != NULL) delete schemas;
    if(approximation != NULL) delete approximation;    
}