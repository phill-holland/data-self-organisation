#include "score.h"
#include <iostream>

void organisation::score::reset(int length)
{
	init = false; cleanup();
	
	this->length = length;

	scores = new float[length];
	if(scores == NULL) return;

	clear();

	init = true;
}

void organisation::score::clear()
{
    for(int i = 0; i < length; ++i)
    {
        scores[i] = 0.0f;
    }
}

void organisation::score::compute(std::string expected, std::string value)
{
    auto occurances = [](std::string to_find, std::string value) 
	{ 
		int result = 0;
		int idx = 0;

        for(int i = 0; i < value.size(); ++i)
        {
			if(value[i] == to_find[idx]) 
			{
				++idx;
				if(idx >= to_find.size())
				{
					idx = 0;
					++result;
				}
			}
			else idx = 0;
        }

        return result;
	};

	auto first_position = [](std::string to_find, std::string value) 
	{ 
        for(int i = 0; i < value.size() - 1; i += 2)
        {
			if((value[i] == to_find[0]) && (value[i + 1] == to_find[1]))
			{
				return i;
			}
        }

        return -1;
	};

	auto dis = [](float f1, float f2, float offset)
	{
		float distance = (float)abs((f1 - f2) / 2) + offset;

		if(distance > 5.0f) distance = 5.0f;
		if((f1 == -1)||(f2 == -1)) distance = 5.0f;
		float result = ((distance / 5.0f) * -1.0f) + 1.0f;
		
		if(result < 0.0f) result = 0.0f;
		if(result > 1.0f) result = 1.0f;

		return result;
	};

    clear();

    if(value.size() > 0)
	{		
      	const int a_len = 6;
        const std::string alphabet[] = { "he", "ll", "o ", "wo", "rl", "d!" };
        #warning fuck knows about position!
		const int position[] = { 0, 2, 4, 6, 8, 10 };

		std::string comp("hello world!");
		int l1 = comp.size();
		int l2 = value.size();
			
		int d = abs(l2 - l1);
		if(d > 50) d = 50;

		float len_score = ((d / 50.0f) * -1.0f) + 1.0f;
		scores[length - 1] = len_score;

		float o_total = 0.0f;

		for(int i = 0; i < a_len; ++i)
		{
			int f1 = first_position(alphabet[i], value);
			int f3 = position[i];
			
			float t = 0.0f;
			int o = occurances(alphabet[i], value);
			if(o > 6) o = 6;
			if(o > 0)
			{ 
				t = (((o - 1) / 5.0f) * -1.0f) + 1.0f;
			} else t = 0.1f;

			o_total += t;

			if(f1 >= f3) scores[i] = dis(f1, f3, 0.0f);
			else scores[i] = 0.1f;
		}

		o_total /= (float)a_len;
		for(int i = 0; i < a_len; ++i)
		{
			scores[i] *= o_total;
		}

		for(int i = 0; i < a_len - 1; ++i)
		{
			int f1 = first_position(alphabet[i], value);
			int f2 = first_position(alphabet[i+1], value);			
			
			if(f1 < f2) scores[i + a_len] = dis(f1, f2, -1.0f);			
		}

		int f1 = first_position(alphabet[a_len - 1], value);
		int f2 = l2;

		if(f1 < f2) scores[a_len + a_len - 1] = dis(f1, f2, -1.0f);			
	}
}

float organisation::score::sum()
{
    float result = 0.0f;

    for(int i = 0; i < length; ++i)
    {
        result += scores[i];
    }
    
    return result / ((float)length);
}

float organisation::score::get(int index)
{
	if((index < 0)||(index >= length)) return 0.0f;

	return scores[index];
}