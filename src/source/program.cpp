#include "program.h"
#include <stack>
#include <unordered_map>
#include <iostream>
#include <tuple>

std::mt19937_64 organisation::program::generator(std::random_device{}());

void organisation::program::reset()
{
    init = false; cleanup();

    length = WIDTH * HEIGHT * DEPTH;

    cells = new cell[length];
    if(cells == NULL) return;

    clear();

    init = true;
}

void organisation::program::clear()
{    
    for(int i = 0; i < length; ++i)
    {
        cells[i].clear();
    }
}

void organisation::program::generate(data &source)
{
    clear();

    for(int i = 0; i < length; ++i)
    {
        int j = (std::uniform_int_distribution<int>{0, 1})(generator);
        if(j == 1)
        {
            int k = (std::uniform_int_distribution<int>{1, source.maximum()})(generator);
            cells[i].generate(k);
        }
    }
}

void organisation::program::mutate()
{
    int j = (std::uniform_int_distribution<int>{0, length - 1})(generator);

    cells[j].mutate();
}

std::string organisation::program::run(int start, data &source)
{
    std::vector<int> result;

    int x = WIDTH - 1 - start;
    if(x < 0) return std::string("");
    
    int y = HEIGHT - 1;
    int z = DEPTH - 1;

    const int MAX = 200;

    std::vector<std::tuple<vector,vector>> positions;
    positions.push_back(std::tuple<vector,vector> (vector { x,y,z },vector {0,1,0}));

    int counter = 0;
    while ((!(positions.empty()))&&(counter < MAX))
    {
        for(std::vector<std::tuple<vector,vector>>::iterator it = positions.begin(); it != positions.end(); ++it)
        {
            std::tuple<vector,vector> temp = positions.back();

            vector current = std::get<0>(temp);
            vector next = std::get<1>(temp);

            positions.pop_back();

            int index = (current.z * WIDTH * HEIGHT) + (current.y * WIDTH) + current.x;
            if(cells[index].is_input(next))
            {
                if(cells[index].value != 0)
                {
                    result.push_back(cells[index].value - 1);
                    ++counter;
                }
                
                std::vector<vector> outputs = cells[index].outputs();
                for(std::vector<vector>::iterator ij = outputs.begin(); ij != outputs.end(); ++ij)
                {
                    vector t = *ij;
                    vector position = { current.x + t.x, current.y + t.y, current.z + t.z };

                    if((position.x >= 0)&&(position.y >= 0)&&(position.z >= 0))
                    {
                        if((position.x < WIDTH)&&(position.y < HEIGHT)&&(position.z < DEPTH))
                        {
                            positions.push_back(std::tuple<vector,vector>(position, t));
                        }
                    }
                }
            }
        }
    };
    
    return source.get(result);
}

int organisation::program::count()
{
    int result = 0;

    for(int i = 0; i < length; ++i)
    {
        if(cells[i].value == 0) ++result;
    }

    return result;
}

void organisation::program::copy(const program &source)
{
    for(int i = 0; i < length; ++i)
    {
        cells[i] = source.cells[i];
    }
}

void organisation::program::cross(program &a, program &b)
{
	int start_a = 0, length_a = 0;
	int start_b = 0, length_b = 0;

	start_a = (std::uniform_int_distribution<int>{0, (int)(a.length - 1)})(generator);
	length_a = (std::uniform_int_distribution<int>{(int)(start_a + 1), a.length})(generator) - start_a;

	start_b = (std::uniform_int_distribution<int>{0, (int)(b.length - 1L)})(generator);
	length_b = (std::uniform_int_distribution<int>{(int)(start_b + 1), b.length})(generator) - start_b;

	int index = 0;
	for (int i = 0; i < start_a; ++i)
	{
		cells[index] = a.cells[i];
		++index;
	}

	for (int i = 0; i < length_b; ++i)
	{
		cells[index] = b.cells[i + start_b];
		++index;
	}

	int offset = a.length - (start_a + length_a);
	for (int i = 0; i < offset; ++i)
	{
		cells[index] = a.cells[i + start_a + length_a];
		++index;
	}
}


void organisation::program::makeNull()
{
    cells = NULL;
}

void organisation::program::cleanup()
{
    if (cells != NULL) delete[] cells;
}