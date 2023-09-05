#include "population.h"
#include "data.h"
#include "history.h"
#include "general.h"
#include <iostream>
#include <string.h>

using namespace std;

std::string source = R"(daisy daisy give me your answer do .
)";
/*I'm half crazy for the love of you .
it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";*/

void run()
{     
    auto strings = organisation::split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do ." };//, "I'm half crazy for the love of you ." };

    int epochs = expected.size();

    organisation::data data(strings);
    organisation::population p(expected, 10000);
    
    p.generate(data);
    organisation::schema best = p.go(data, expected, 100);

    for(int i = 0; i < epochs; ++i)
    {
        organisation::history history;

        std::string output = best.run(i, expected[i], data, &history);
        std::cout << "\r\n" << output << "\r\n";
        std::cout << history.get(data);
        
    }
}

int main(int argc, char *argv[])
{  
    run();

    return 0;
}