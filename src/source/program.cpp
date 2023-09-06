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
        //int j = (std::uniform_int_distribution<int>{0, 1})(generator);
        //if(j == 1)
        //{
            int k = (std::uniform_int_distribution<int>{1, source.maximum()})(generator);
            cells[i].generate(k);
        //}
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

    x = (WIDTH / 2) - start;
    y = (HEIGHT / 2);
    z = (DEPTH / 2);
    
    const int MAX = 50;//100;

    std::vector<std::tuple<vector,vector>> positions;
    positions.push_back(std::tuple<vector,vector> (vector { x,y,z },vector {0,-1,0}));

    int counter = 0;
    while ((!(positions.empty()))&&(counter < MAX))
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

        ++counter;
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
    if(count() <= 0) 
        return false;

    int gates = 0;
    for(int i = 0; i < length; ++i)
    {
        std::tuple<bool,bool> temp = cells[i].validate(source.maximum());
        if(!std::get<0>(temp)) return false;
        if(std::get<1>(temp)) ++gates;
    }

    if(gates <= 0) 
        return false;

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
    if((a.length != b.length)||(length != a.length)) return;

    int a1 = 0, b1 = 0;
    if(middle < 0)
    {
        while(a1 == b1)
        {
            a1 = (std::uniform_int_distribution<int>{0, (int)(length - 1)})(generator);
            b1 = (std::uniform_int_distribution<int>{0, (int)(length - 1)})(generator);
        };

        if(a1 > b1)
        {
            int temp = a1;
            a1 = b1;
            b1 = temp;
        }
    }
    else
    {
        a1 = middle;
        b1 = length;
    }

    int index = 0;
    for(int i = 0; i < a1; ++i)
    {
        cells[index].copy(a.cells[i]);
		++index;
    }

    for(int i = a1; i < b1; ++i)
    {
        cells[index].copy(b.cells[i]);
        ++index;
    }

    for(int i = b1; i < length; ++i)
    {
        cells[index].copy(a.cells[i]);
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