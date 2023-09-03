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

void organisation::program::mutate(data &source)
{
    int j = (std::uniform_int_distribution<int>{0, length - 1})(generator);

    cells[j].mutate(source.maximum());
}

std::string organisation::program::run(int start, data &source, history *destination)
{
    std::vector<int> result;

    int x = WIDTH - 1 - start;
    if(x < 0) return std::string("");
    
    int y = HEIGHT - 1;
    int z = DEPTH - 1;

    const int MAX = 200;

    std::vector<std::tuple<vector,vector>> positions;
    positions.push_back(std::tuple<vector,vector> (vector { x,y,z },vector {0,-1,0}));

    int counter = 0;
    while ((!(positions.empty()))&&(counter < MAX))
    {        
        while(!positions.empty())
        {
            std::tuple<vector,vector> temp = positions.back();

            vector current = std::get<0>(temp);
            vector next = std::get<1>(temp);

            positions.pop_back();

            int index = (current.z * WIDTH * HEIGHT) + (current.y * WIDTH) + current.x;
           
            vector input = next.normalise().inverse();
            if(cells[index].is_input(input))
            {
                if(destination != NULL) destination->push(current, cells[index].value);

                if(!cells[index].is_empty())
                {                
                    result.push_back(cells[index].value);
                    ++counter;
                }
                
                std::vector<vector> outputs = cells[index].outputs(input);
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
        if(cells[i].value > -1) ++result;
    }

    return result;
}

void organisation::program::set(int value, int x, int y, int z)
{
    if ((x < 0)||(x >= WIDTH)) return;
    if ((y < 0)||(y >= HEIGHT)) return;
    if ((z < 0)||(z >= DEPTH)) return;

    int index = (z * WIDTH * HEIGHT) + (y * WIDTH) + x;

    cells[index].value = value;
}

void organisation::program::set(vector input, vector output, int magnitude, int x, int y, int z)
{
    if ((x < 0)||(x >= WIDTH)) return;
    if ((y < 0)||(y >= HEIGHT)) return;
    if ((z < 0)||(z >= DEPTH)) return;

    int index = (z * WIDTH * HEIGHT) + (y * WIDTH) + x;

    cells[index].set(input, output, magnitude);
}

bool organisation::program::validate(data &source)
{
    if(count() <= 0) return false;

    int gates = 0;
    for(int i = 0; i < length; ++i)
    {
        std::tuple<bool,bool> temp = cells[i].validate(source.maximum());
        if(!std::get<0>(temp)) return false;
        if(std::get<1>(temp)) ++gates;
    }

    if(gates <= 0) return false;

    return true;
}

void organisation::program::copy(const program &source)
{
    for(int i = 0; i < length; ++i)
    {
        cells[i] = source.cells[i];
    }
}

bool organisation::program::equals(const program &source)
{
    for(int i = 0; i < length; ++i)
    {
        if(!cells[i].equals(source.cells[i])) return false;
    }

    return true;
}

void organisation::program::cross(program &a, program &b, int middle)
{
	int start_a = 0, length_a = 0;
	int start_b = 0, length_b = 0;

    if(middle < 0)
    {
        start_a = (std::uniform_int_distribution<int>{0, (int)(a.length - 1)})(generator);
        length_a = (std::uniform_int_distribution<int>{(int)(start_a + 1), a.length})(generator) - start_a;

        start_b = (std::uniform_int_distribution<int>{0, (int)(b.length - 1L)})(generator);
        length_b = (std::uniform_int_distribution<int>{(int)(start_b + 1), b.length})(generator) - start_b;
    }
    else
    {
        start_a = middle;
        length_a = length - middle;

        start_b = middle;
        length_b = length - middle;
    }

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