#include "generator.h"
#include "population.h"
#include "semaphore.h"

void organisation::generator::background(core::threading::thread *bt)
{
	sleep(50);

    //for(int i = 0; i < 10; ++i)
    //{
        schema temp = get();	
        if(!pop->outgoing.set(temp)) 
            sleep(100);
    //}
}

organisation::schema organisation::generator::get()
{   
    organisation::schema result;
    if(!pop->get(result))
        std::cout << "moo\r\n";
    return result; 
    /*
    organisation::schema result;

    const float mutate_rate_in_percent = 20.0f;
    const float mutation = (((float)pop->size) / 100.0f) * mutate_rate_in_percent;

    int t = (std::uniform_int_distribution<int>{0, pop->size - 1})(pop->generator);

	threading::semaphore lock(pop->token);

    if(((float)t) <= mutation) 
    {
        schema *s1 = pop.best();

        result.copy(*s1);
        result.mutate(pop->mappings);            
    }
    else
    {
        schema *s1 = pop->best();
        schema *s2 = pop->best();
             
        s1->cross(&result, s2);
    }

    return result;
    */
}
