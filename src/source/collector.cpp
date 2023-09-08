#include "collector.h"
#include "population.h"
#include "semaphore.h"
#include <chrono>

using namespace std::chrono;

void organisation::collector::background(core::threading::thread *bt)
{
	sleep(50);

    if(pop->incoming.entries() > 0)
    {
        //high_resolution_clock::time_point begin = high_resolution_clock::now();
		//duration<double> time_span = duration_cast<duration<double>>(now - previous);
		
         threading::semaphore lock(pop->token);
        int i = 0;
        while((pop->incoming.get(temp))&&(i++ < 50))
        //if(pop->incoming.get(temp))
        {
            

            //int offspring = pop->worst();
            //pop->set(offspring, temp);

        ++counter;
        if(counter > 200)
        {
            high_resolution_clock::time_point end = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(end - previous);
            double val = time_span.count();

            std::cout << "set " << val << "\r\n";
            counter = 0;
            previous = end;
        }

        }
    }
    //schema temp = pop->get();
    //if(!pop->outgoing.set(temp)) sleep(100);
}
