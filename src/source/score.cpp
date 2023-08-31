#include "score.h"
#include <iostream>
#include <tuple>
#include <sstream>

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

//bool scoring::general::compute(brain::score::scores &results, string comparison, string value, int score_offset)
void organisation::score::compute(std::string expected, std::string value)
{
	auto _words = [](std::string source)
    {
        std::stringstream stream(source);  
        std::string word;

        int index = 0;
        while (stream >> word) 
        {
			++index;
        };

        return index;
    };

    auto _split = [](std::string source)
    {
        std::vector<std::tuple<std::string,int>> result;

        std::stringstream stream(source);  
        std::string word;

        int index = 0;
        while (stream >> word) 
        {
            result.push_back(std::tuple<std::string,int>(word,index));
			++index;
        };

        return result;
    };

	auto _occurances = [](std::string to_find, std::string value) 
	{ 
		std::stringstream stream(value);
		std::string word;

		int result = 0;
	
		while(stream >> word)
		{
			if(to_find.compare(word) == 0) ++result;
		}

		return result;
	};

	auto _closest = [](std::tuple<std::string,int> to_find, std::string value, int alphabet_length)
	{		
		std::stringstream stream(value);
		std::string word;

		std::string find = std::get<0>(to_find);
		int position = std::get<1>(to_find);

		int distance = alphabet_length + 1;
		int result = -1;

		int count = 0;

		while(stream >> word)
		{
			if(find.compare(word) == 0)
			{
				int temp = abs(count - position);
				if(temp < distance) 
				{
					distance = temp;
					result = count;
				}
			}
			++count;
		}

		return result;
	};

	auto _distance = [](float f1, float f2, float offset, float max)
	{
		float distance = (float)abs(f1 - f2) + offset;

		if(distance > max) distance = max;
		if((f1 == -1)||(f2 == -1)) distance = max;
		float result = ((distance / max) * -1.0f) + 1.0f;
		
		if(result < 0.0f) result = 0.0f;
		if(result > 1.0f) result = 1.0f;

		return result;
	};

	//bool valid = true;
	const int MAX_WORDS = 5;

    if(value.size() > 0)
	{		
        std::vector<std::tuple<std::string,int>> alphabet = _split(expected);
        
		const int alphabet_len = alphabet.size();		
		float max = (float)MAX_WORDS;
		if(((float)(alphabet_len - 1))>max) max = (float)(alphabet_len - 1);

		int l1 = expected.size();
		int l2 = value.size();

		const float fib[] = { 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 89.0f, 144.0f };
		const int max_str_len = 10;
		//const int max_str_len = 50;	
	
		int d = abs(l2 - l1);
		if(d > max_str_len) d = max_str_len;

		//float len_score = ((d / ((float)max_str_len)) * -1.0f) + 1.0f;		
		float len_score = 1.0f / fib[d];
		//if(!results.set(len_score, results.size() - 1)) valid = false;
		scores[length - 1] = len_score;
		
        int index = 0;
		for(auto &it : alphabet)
		{            
			int f1 = _closest(it, value, alphabet_len);
			int f3 = std::get<1>(it);
						
			if(f1 >= f3) 
            {
				float distance = _distance(f1, f3, 0.0f, max); 
                //if(!results.set(distance, index + score_offset)) valid = false;
				scores[index] = distance;
            }
			else scores[index] = 0.1f;
			//else if(!results.set(0.1f, index + score_offset)) valid = false;

            ++index;
		}
	
        index = 0;
        for(std::vector<std::tuple<std::string,int>>::iterator it = alphabet.begin(); it != (alphabet.end() - 1); ++it)
		{			
            std::tuple<std::string,int> a1 = *it;
            std::tuple<std::string,int> a2 = *(it + 1);

			int f1 = _closest(a1, value, alphabet_len);
			int f2 = _closest(a2, value, alphabet_len);			
			
			if(f1 < f2) 
			{
				//if(!results.set(_distance(f1, f2, -1.0f, max),index + alphabet_len + score_offset)) valid = false;
				scores[index + alphabet_len] = _distance(f1, f2, -1.0f, max);
			}

            ++index;
		}

        std::tuple<std::string,int> last = *(alphabet.end() - 1);
		int f1 = _closest(last, value, alphabet_len);
		int f2 = _words(value);

		if(f1 < f2) 
		{
			//if(!results.set(_distance(f1, f2, -1.0f, max),alphabet_len + alphabet_len - 1 + score_offset)) valid = false;
			scores[alphabet_len + alphabet_len - 1] = _distance(f1, f2, -1.0f, max);
		}
	}

	//return valid;
}
/*
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
*/
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