#include "population.h"
#include "data.h"
#include "history.h"
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

    int epochs = expected.size();

    organisation::data data(strings);
    organisation::population p(expected, 2000);
    
    p.generate(data);
    organisation::schema best = p.go(data, expected, 1000);

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