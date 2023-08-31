#include "population.h"
#include "data.h"
#include <iostream>
#include <string.h>

using namespace std;

std::string source = R"(daisy daisy give me your answer do .
I'm half crazy for the love of you .
it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
)";

void run()
{     
    auto split = [](std::string source)
    {
        std::vector<std::string> result;
        std::string temp; 

        for(auto &ch: source)
        {
            if((ch != ' ')&&(ch != 10)&&(ch != 13))
            {
                temp += ch;
            }
            else
            {
                if(temp.size() > 0)
                {
                    result.push_back(temp);
                    temp.clear();
                }
            }
        }

        if(temp.size() > 0) result.push_back(temp);
        
        return result;
    };

    auto strings = split(source);
    
    std::vector<std::string> expected = { "daisy daisy give me your answer do ." };

    organisation::data data(strings);
    organisation::population p(expected, 2000);

    
    p.generate(data);
    organisation::schema best = p.go(data, expected, 1000);

    //std::cout << best.output();
    //std::tuple<std::string, bool, int, int> result = best.run();
    //std::cout << std::get<0>(result);
}

int main(int argc, char *argv[])
{  
    run();

    return 0;
}