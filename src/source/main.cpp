#include "population.h"
#include "data.h"
#include "history.h"
#include "general.h"
#include <iostream>
#include <string.h>

#include "fifo.h"
#include "schema.h"

using namespace std;

std::string source = R"(daisy daisy give me your answer do .)";
/*I'm half crazy for the love of you .)";
/*it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";*/

void run(int rerun = 1)
{     
    auto strings = organisation::split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do ." };//, "I'm half crazy for the love of you ." };

    int epochs = expected.size();

    organisation::data data(strings);
    organisation::population p(data, expected, 1000);//3000);//2000);//1000);
    
    const int iterations = 300;

    for(int j = 0; j < rerun; ++j)
    {        
        int actual = 0;

        p.clear();
        p.generate();
        p.start();

        organisation::schema best = p.go(expected, actual, iterations);

        for(int i = 0; i < epochs; ++i)
        {
            organisation::history history;

            std::string output = best.run(i, expected[i], data, &history);
            std::cout << "\r\n" << output << "\r\n";
            std::cout << history.get(data);

            if(actual <= iterations) 
            {
                std::string filename("output");
                filename += std::to_string(i);
                filename += std::string(".csv");

                history.append(filename, data);
            }
        }        
    }
}

int main(int argc, char *argv[])
{  
    run(200);
/*
core::queue::fifo<organisation::schema,10L> fifo;

organisation::schema *temp = new organisation::schema();

fifo.set(*temp);

organisation::schema *moo = new organisation::schema();
fifo.get(*moo);

delete moo;
delete temp;
*/
    return 0;
}