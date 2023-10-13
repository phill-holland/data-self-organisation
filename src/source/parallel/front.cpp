#include "parallel/front.hpp"
#include "score.h"

void organisation::parallel::front::reset(::parallel::device &dev, int terms, int clients)
{
    init = false; cleanup();

    this->dev = &dev;
    
    this->terms = terms;
    this->clients = clients;

    int length = terms * clients;
    
    schemas = new organisation::schema*[clients];
    if(schemas == NULL) return;

    for(int i = 0; i < clients; ++i) schemas[i] = NULL;

    sycl::queue q = ::parallel::queue(dev).get();
    
    deviceValues = sycl::malloc_device<float>(length, q);
    if(deviceValues == NULL) return;

    deviceIsFront = sycl::malloc_device<int>(clients, q);
    if(deviceIsFront == NULL) return;
    
    deviceDominatedCount = sycl::malloc_device<int>(clients, q);
    if(deviceDominatedCount == NULL) return;

    hostIsFront = sycl::malloc_host<int>(clients, q);
    if(hostIsFront == NULL) return;    

    hostDominatedCount = sycl::malloc_host<int>(clients, q);
    if(hostDominatedCount == NULL) return;

    hostValues = sycl::malloc_host<float>(length, q);
    if(hostValues == NULL) return;

    init = true;
};

void organisation::parallel::front::clear()
{
    memset(hostValues, 0, sizeof(float) * terms * clients);
}

void organisation::parallel::front::set(organisation::schema *source, int client)
{
    if((client < 0) || (client > clients)) return;

    schemas[client] = source;

    int index = 0;
    int epochs = source->scores.size();
    for(int epoch = 0; epoch < epochs; ++epoch)
    {
        organisation::score temp = source->scores[epoch];
        for(int i = 0; i < temp.size(); ++i)
        {
            if(index >= terms) return;

            float score = temp.get(i);
            hostValues[(client * terms) + index] = score;
            ++index;
        }
    }
}

void organisation::parallel::front::set(int *source, int client)
{
    if((client < 0) || (client > clients)) return;

    for(int i = 0; i < terms; ++i)
    {
        float score = (float)source[i];
        hostValues[(client * terms) + i] = score;
    }
}

/*
	auto is_dominant = [](int *a, int *b) 
	{ 
		const int dimensions = 2;
		bool any = false;
		for (int i = 0; i < dimensions; ++i)
		{
			//Log << a[i];
			// modify kdtree for this condition, between left and right insert
			if(a[i] > b[i]) return false;
			any |= (a[i] < b[i]);
			//if((*epochs)[i]->results.score > (*source.epochs)[i]->results.score) return false;
			//any |= ((*epochs)[i]->results.score < (*source.epochs)[i]->results.score);
		}

		return any;
	};
*/

organisation::schema *organisation::parallel::front::get(int client)
{
    if((client < 0) || (client > clients)) return NULL;

    return schemas[client];
}

bool organisation::parallel::front::is_front(int client)
{
    if((client < 0) || (client > clients)) return false;

    return hostIsFront[client] == 1 ? true : false;
}

int organisation::parallel::front::rank(int client)
{
    if((client < 0) || (client > clients)) return -1;

    return hostDominatedCount[client];
}

int organisation::parallel::front::front_count()
{
    int result = 0;

    for(int i = 0; i < clients; ++i)
    {
        if(hostIsFront[i] == 1) ++result;
    }

    return result;
}

int organisation::parallel::front::rank_front_count()
{
    int result = 0;

    for(int i = 0; i < clients; ++i)
    {
        if(hostDominatedCount[i] == 0) ++result;
    }

    return result;
}

void organisation::parallel::front::run(::parallel::queue *q)
{
    const size_t length = clients;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    sycl::range global{0,length};
    sycl::range local {1,(size_t)terms};

    auto m1 = qt.memcpy(deviceValues, hostValues, sizeof(float) * terms * clients);
    auto m2 = qt.memset(deviceIsFront, 0, sizeof(int) * clients);
    auto m3 = qt.memset(deviceDominatedCount, 0, sizeof(int) * clients);

    m1.wait();
    m2.wait();
    m3.wait();

    qt.submit([&](auto &h) 
    {        
        auto _values = deviceValues;
        auto _isFront = deviceIsFront;
        auto _dominatedCount = deviceDominatedCount;
        auto _terms = terms;
        auto _clients = clients;

//sycl::stream out(2048, 256, h);

        h.parallel_for(num_items, [=](auto i) 
        {
            int index = i * _terms;
            int front = 1;
            int count = 0;

            for(int client = 0; client < _clients; ++client)
            {
                if(client != i)                
                {                 
                    bool any = false;

                    for(int j = 0; j < _terms; j = j + 1)
                    {                        
                        float a = _values[j + index];           
                        float b = _values[j + (client * _terms)];

                        if(a > b) 
                        { 
                            any = false;
                            break; 
                        }
			            
                        any |= (a < b);
                    }

                    if(any) 
                    { 
                        front = 0; 
                        count = count + 1;
                    }
                }
            }

            _isFront[i] = front;
            _dominatedCount[i] = count;
        });
    }).wait();

    auto m4 = qt.memcpy(hostIsFront, deviceIsFront, sizeof(int) * clients);
    auto m5 = qt.memcpy(hostDominatedCount, deviceDominatedCount, sizeof(int) * clients);

    m4.wait();
    m5.wait();
}

void organisation::parallel::front::outputarb(int *source, int length)
{
	int *temp = new int[length];
	if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(int) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
		if ((temp[i] != -1)&&(temp[i]!=0))
		{
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i]);
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::front::outputarb(sycl::float4 *source, int length)
{
    sycl::float4 *temp = new sycl::float4[length];
    if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(sycl::float4) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
        int ix = (int)(temp[i].x() * 100.0f);
        int iy = (int)(temp[i].y() * 100.0f);
        int iz = (int)(temp[i].z() * 100.0f);

        if ((ix != 0) || (iy != 0) || (iz != 0))
        {
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i].x());
			result += std::string(",");
			result += std::to_string(temp[i].y());
			result += std::string(",");
			result += std::to_string(temp[i].z());
			result += std::string(",");
            result += std::to_string(temp[i].w());
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	    
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::front::outputarb(float *source, int length)
{
    float *temp = new float[length];
    if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(float) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
        int ix = (int)(temp[i] * 100.0f);

        if (ix != 0)
        {
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i]);
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	    
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::front::makeNull()
{
    dev = NULL;

    schemas = NULL;

    deviceValues = NULL;   
    deviceIsFront = NULL;
    deviceDominatedCount = NULL;
    hostIsFront = NULL;
    hostDominatedCount = NULL;
    hostValues = NULL;    
}

void organisation::parallel::front::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();
        
        if (hostValues != NULL) sycl::free(hostValues, q);
        if (hostDominatedCount != NULL) sycl::free(hostDominatedCount, q);
        if (hostIsFront != NULL) sycl::free(hostIsFront, q);
        if (deviceDominatedCount != NULL) sycl::free(deviceDominatedCount, q);
        if (deviceIsFront != NULL) sycl::free(deviceIsFront, q);
        if (deviceValues != NULL) sycl::free(deviceValues, q);        
    }

    if(schemas != NULL) delete[] schemas;
}