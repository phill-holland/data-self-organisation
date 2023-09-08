#include "population.h"
#include "general.h"
#include <iostream>
#include <fcntl.h>
#include <future>
#include <algorithm>
#include <signal.h>

std::mt19937_64 organisation::population::generator(std::random_device{}());
/*
void organisation::population::background(core::threading::thread *bt)
{
	sleep(1);

    schema temp = get();
    //std::cout << "set " << outgoing.entries() << "\r\n";
    if(!outgoing.set(temp)) 
    {
        //std::cout << "full\r\n";
        sleep(10);
    }
    //std::cout << "set end\r\n";
}
*/
void organisation::population::reset(organisation::data source, std::vector<std::string> expected, int size)
{
    init = false; cleanup();
    this->size = size;

    mappings = source;

    dimensions = 0;
    std::vector<std::vector<std::string>> d;
    for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
    {
        std::vector<std::string> t = split(*it);
        //lengths.push_back((t.size() * 2) + 1);
        dimensions += (t.size() * 2) + 1;
    }

	approximation = new dominance::kdtree::kdtree(dimensions, 10000);
	if (approximation == NULL) return;
	if (!approximation->initalised()) return;

    schemas = new organisation::schemas(size);
    if (schemas == NULL) return;
    if (!schemas->initalised()) return;

    init = true;
}

void organisation::population::clear()
{
    schemas->clear();
    #warning need to clear kdtree
}

void organisation::population::generate()
{
    schemas->generate(mappings);
}

organisation::schema organisation::population::go(std::vector<std::string> expected, int &count, int iterations)
{
    schema res;
    float most = 0.0f;

    bool result = false;
    count = 0;

    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)size) / 100.0f) * mutate_rate_in_percent;

    //schema **source = left, **destination = right;
    //generate();

    do
    {
        float total = 0.0f;        
      
        for(int generation = 0; generation < size; generation += threads)
        {        
            std::vector<std::future<std::tuple<std::vector<std::string>,float>>> results;

            int m = threads;
            if(generation + threads >= size) 
                m = size - generation;
            for(int i = 0; i < m; ++i)
            {
             //std::cout << "start " << intermediate[i] << "\r\n";   
                //std::cout << (i + generation) << "\r\n";
                auto result = std::async(&organisation::population::run, this, expected, mutation);
                results.push_back(std::move(result));
            }

            /*
            for (std::vector<std::future<std::tuple<std::vector<std::string>,schema*>>>::iterator it = results.begin(); it != results.end(); ++it)
            {
                it->wait();
            }
            */
            

            for (std::vector<std::future<std::tuple<std::vector<std::string>,float>>>::iterator it = results.begin(); it != results.end(); ++it)
            {
                std::tuple<std::vector<std::string>,float> outputs = it->get();

                std::vector<std::string> output = std::get<0>(outputs);
                float sum = std::get<1>(outputs);
                //schema *dest = std::get<1>(outputs);
                
                //int offspring = worst();
                //set(offspring, *dest);
                //incoming.set(*dest);
                
                //std::cout << "end " << dest << "\r\n";
/*
                std::vector<std::string> output = std::get<0>(outputs);
                schema *dest = std::get<1>(outputs);
        
                int offspring = worst();
                set(offspring, *dest);*/
                //float sum = dest->sum();
                total += sum;

                if(sum > most)
                {
                    //res.copy(*dest);
                    most = sum;
                }

                if(sum >= 0.9999f) result = true;    
                if(output == expected) result = true;   
                          
            }


           // int blocks_len = size / threads;
            // find worst within start,end of thread block, in source
            // find best within destination (throughout whole)
            // overwrite worst with best

            //int block_len = 
/*
            while(incoming.entries()>0)
            {
                sleep(2);
            }*/
        }

        //int m = threads;
        //if(generation + threads >= size) 
            //m = size - generation;
      //  for(int i = 0; i < threads; ++i)
        //{
        //std::cout << "start " << intermediate[i] << "\r\n";   
            //std::cout << (i + generation) << "\r\n";
            //auto result = std::async(&organisation::population::run, this, destination[generation + i], source, expected, mutation);
            //results.push_back(std::move(result));
          //  auto result = std::async(&organisation::population::back, this, source, destination, i);
        //}

        //schema **tt = source;
        //source = destination;
        //destination = tt;

        total /= size;
        
        std::cout << "Generation (" << count << ") Best=" << most;
        std::cout << " Avg=" << total;
        std::cout << " IC=" << incoming.entries();
        std::cout << " OC=" << outgoing.entries();
        std::cout << "\r\n";

        if((iterations > 0)&&(count > iterations)) result = true;

        ++count;

    } while(!result);

    return res;
}

std::tuple<std::vector<std::string>,float> organisation::population::run(std::vector<std::string> expected, const float mutation)
{    
    std::vector<std::string> results;

//organisation::schema temp;
//temp = get(source);
    //if(outgoing.get(*destination))
    //*destination = get(source);
    //destination->scores.clear();
    //destination->clear();
    //*destination[generation + i] = get(source);
    //{
        organisation::schema destination;
        if(!get(destination)) return std::tuple<std::vector<std::string>,float>(results,0.0f);        
        //while(!outgoing.get(destination)) { };
        //return std::tuple<std::vector<std::string>,float>(results,0.0f);

        int epoch = 0;
        for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
        {
            results.push_back(destination.run(epoch, *it, mappings));
            //results.push_back(temp.run(epoch, *it, *source));
            ++epoch;
        }
    //}

  //  if(destination->sum() < temp.sum())
    //    *destination = temp;

    set(destination);

    return std::tuple<std::vector<std::string>,float>(results,destination.sum());
/*    
    int t = (std::uniform_int_distribution<int>{0, size - 1})(generator);

    destination->clear();

    if(((float)t) <= mutation) 
    {
        schema *s1 = best();

        destination->copy(*s1);
        destination->mutate(*source);
        
        int epoch = 0;
        for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
        {
            results.push_back(destination->run(epoch, *it, *source));
            ++epoch;
        }
    }
    else
    {
        schema *s1 = best();
        schema *s2 = best();
             
        s1->cross(destination, s2);
        
        int epoch = 0;
        for(std::vector<std::string>::iterator it = expected.begin(); it != expected.end(); ++it)
        {
            results.push_back(destination->run(epoch, *it, *source));
            ++epoch;
        }
    }

    return std::tuple<std::vector<std::string>,schema*>(results,destination);
    */
}
/*
void organisation::population::back(schema **destination, schema **source, int thread)
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
organisation::schema organisation::population::top()
{
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
   return organisation::schema();
}

bool organisation::population::get(schema &destination)
{
    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, size - 1})(generator);

    if(((float)t) <= mutation) 
    {
        schema s1;
        if(!best(s1)) return false;

        destination.copy(s1);
        destination.mutate(mappings);            
    }
    else
    {
        schema s1,s2;

        if(!best(s1)) return false;
        if(!best(s2)) return false;
                     
        s1.cross(&destination, &s2);
    }

    return true;
}

/*
organisation::schema *organisation::population::best(schema **source)
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

int organisation::population::worst(schema **source, int start, int end)
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

bool organisation::population::best(organisation::schema &destination)
{
    const int samples = 10;

	//std::uniform_int_distribution<int> rand{ 0, size - 1 };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    //long competition;

    //long best = rand(generator);    
    /*
    organisation::schema best, competition;
    int escape = 0;
    while((!schemas->get(best, rand(generator)))&&(++escape<15)) { }
    if(escape>=15) return false;
    */
   organisation::schema competition;
   if(pull(destination)<0) return false;

	float score = destination.sum();//source[best]->sum();

	for (int i = 0; i < samples; ++i)
	{
		//competition = rand(generator);
        if(pull(competition)<0) return false;

        destination.get(temp1, minimum, maximum);
        competition.get(temp2, minimum, maximum);

		float t2 = competition.sum();

		if(approximation->exists(temp1))
		{
			if(approximation->inside(temp1, &origin, &temp2))
			{
				score = t2;
			}
			else
			{
				destination = competition;
				score = t2;
			}
		}
        else if(t2 > score)
		{
			destination = competition;
			score = t2;
		}
	}

    return true;
	//return source[best];
}

int organisation::population::pull(organisation::schema &destination)
{
    std::uniform_int_distribution<int> rand{ 0, size - 1 };
    int escape = 0, index = rand(generator);
    while((!schemas->get(destination, index))&&(++escape<15)) { index = rand(generator); }
    if(escape>=15) return -1;
    return index;
}

int organisation::population::worst(schema &destination)
{
    const int samples = 10;

	//std::uniform_int_distribution<int> rand{ 0L, size - 1 };

	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);
	dominance::kdtree::kdpoint origin(dimensions);

	temp1.set(0L);
	temp2.set(0L);
	origin.set(0L);

    int result = -1;

    //int competition;
    int temp;
    //int worst = rand(generator);
    organisation::schema competition;
    result = pull(destination);
    if(result < 0) return -1;

	float score = destination.sum();//data[worst]->sum();

	for (int i = 0; i < samples; ++i)
	{
		//competition = rand(generator);
        temp = pull(destination);
        if(temp < 0) return -1;

        destination.get(temp1, minimum, maximum);
        competition.get(temp2, minimum, maximum);

		float t2 = competition.sum();

		if(approximation->exists(temp2))
		{
			if(approximation->inside(temp2, &origin, &temp1))
			{
				score = t2;
			}
			else 
			{
				destination = competition;
                result = temp;
				score = t2;
			}
		}
		else if(t2 < score)
		{
			destination = competition;
            result = temp;
			score = t2;
		}
	}

    return result;
    //return true;
	//return worst;
}

bool organisation::population::set(schema &source)//int index)
{    
    organisation::schema destination;
    int index = worst(destination);

    if(index < 0) return false;
	if(source.sum() < destination.sum()) return false;

    std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
    bool result = false;
   
	dominance::kdtree::kdpoint temp1(dimensions), temp2(dimensions);

	temp1.set(0L);
	temp2.set(0L);

    destination.get(temp1, minimum, maximum);
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
    if(!schemas->set(source, index)) return false;

	return result;
}

void organisation::population::makeNull() 
{ 
    approximation = NULL;
    schemas = NULL;
}

void organisation::population::cleanup() 
{ 
    if(schemas != NULL) delete schemas;
    if(approximation != NULL) delete approximation;    
}