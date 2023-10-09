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

    qt.memcpy(deviceValues, hostValues, sizeof(float) * terms * clients).wait();
    qt.memset(deviceIsFront, 0, sizeof(int) * clients).wait();

    qt.submit([&](auto &h) 
    {        
        auto _values = deviceValues;
        auto _isFront = deviceIsFront;
        auto _terms = terms;
        auto _clients = clients;

//sycl::stream out(2048, 256, h);

        h.parallel_for(num_items, [=](auto i) 
        {
            int index = i * _terms;
            int front = 1;

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
                        break;
                    }
                }
            }

            _isFront[i] = front;

            // (if front == 0)
            // points is dominated by another
            // atomic add dominatedCount[i]
        });
    }).wait();

    qt.memcpy(hostIsFront, deviceIsFront, sizeof(int) * clients).wait();

   // outputarb(deviceIsFront, clients);
}

void organisation::parallel::front::extra(::parallel::queue *q)
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
                        //break;
                    }
                }
            }

            _isFront[i] = front;
            _dominatedCount[i] = count;

            // (if front == 0)
            // points is dominated by another
            // atomic add dominatedCount[i]
        });
    }).wait();

    auto m4 = qt.memcpy(hostIsFront, deviceIsFront, sizeof(int) * clients);
    auto m5 = qt.memcpy(hostDominatedCount, deviceDominatedCount, sizeof(int) * clients);

    m4.wait();
    m5.wait();
    //outputarb(deviceIsFront, clients);
    //std::cout << "\r\n\r\n";
    //outputarb(deviceDominatedCount, clients);
}

void organisation::parallel::front::run2(::parallel::queue *q)
{
    const size_t length = clients;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    //sycl::range global{0,length};
    //sycl::range local {1,(size_t)terms};

    qt.memcpy(deviceValues, hostValues, sizeof(float) * terms * clients).wait();
    qt.memset(deviceIsFront, 0, sizeof(int) * clients).wait();

    qt.submit([&](auto &h) 
    {        
        auto _values = deviceValues;
        auto _isFront = deviceIsFront;
        auto _terms = terms;
        auto _clients = clients;

sycl::stream out(65536, 256, h);

        //h.parallel_for(num_items, [=](auto i) 
        h.parallel_for(sycl::nd_range<1>((size_t)length, (size_t)terms ),[=](sycl::nd_item<1> i)//[[intel::reqd_sub_group_size((size_t)8)]]
        {
            int index = i.get_global_id(0);
            int group = index / _terms;
            int group_idx = index % _terms;
            auto sg = i.get_sub_group();
            auto r = sg.get_local_id()[0]; // same as group_idx
            auto d = sg.get_group_id()[0];

            out << "index " << index << " group " << group << " group_idx " << group_idx << " r " << r << " d " << d << "\r\n";

/*
            int index = i.get_global_id(0) * _terms;
            auto sg = i.get_sub_group();
            for(int j = 0; j < _terms; ++j)
            {                        
                //float a = _values[j + index];
                //auto a = sycl::group_broadcast(sg, _values[index], j) * 1.0f; 
                //auto a = select_by_group(sg, _values[index], j);
                auto b = _values[index % _terms];
auto a = sycl::permute_group_by_xor(sg, _values[index], j);
                //if(index % _terms != index + j)
                //if(index != index + j)
                //{
                    out << "index%terms=" << (index % _terms) << " idx+j=" << (index + j) << "\r\n";
                    out << "idx=" << index << " j=" << j << " a=" << a <<  " b=" << b << "\r\n";
                //}
            }
*/
            /*
            auto sg = i.get_sub_group();

            //int index = i * _terms;
            int index = i.get_global_id()[0] * _terms;
            int front = 1;

            for(int client = 0; client < _clients; ++client)
            {
                //if(client != i)
                if(client != i.get_global_id()[0])
                {                 
                    bool any = false;

                    for(int j = 0; j < _terms; j = j + 1)
                    {                        
                        //float a = _values[j + index];
                        auto a = sycl::group_broadcast(sg, _values[index], j) * 1.0f; 
                        float b = _values[j + (client * _terms)];

out << "a=" << a << " b=" << b << " sgid=" << sg.get_local_id()[0] << " " << sg.get_local_range()[0] << "\r\n";
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
                        break;
                    }
                }
            }

            _isFront[i.get_global_id()[0]] = front;
            */
        });
    }).wait();

    outputarb(deviceIsFront, clients);
}

void organisation::parallel::front::run3(::parallel::queue *q)
{
    const size_t length = clients;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    qt.memcpy(deviceValues, hostValues, sizeof(float) * terms * clients).wait();
    qt.memset(deviceIsFront, 0, sizeof(int) * clients).wait();

// ***
    //auto a = dev->local_mem_size();

    size_t wgroup_size = terms;
    size_t part_size = wgroup_size * 2;
    size_t n_wgroups = clients;
    size_t len = clients * terms;
    //auto n_wgroups = (len + part_size - 1) / part_size;
    qt.submit([&] (sycl::handler& cgh) 
    {
        sycl::accessor <float, 1, sycl::access::mode::read_write, 
                                    sycl::access::target::local> 
                        local_mem(sycl::range<1>(wgroup_size * 5), cgh);

        //auto global_mem = buf.get_access<sycl::access::mode::read_write>(cgh);
        auto global_mem = deviceValues;
        auto _isFront = deviceIsFront;
        auto _terms = terms;
        sycl::stream out(65536, 256, cgh);

        cgh.parallel_for<class reduction_kernel>(sycl::nd_range<1>(n_wgroups * wgroup_size, wgroup_size),[=] (sycl::nd_item<1> item) 
        {            
            auto sg = item.get_sub_group();

            size_t local_id = item.get_local_linear_id();
            size_t global_id = item.get_global_linear_id();
            //local_mem[local_id] = 0;

            //if ((2 * global_id) < len) 
            //if(global_id * wgroup_size < wgroup_size)
            //if(global_id * wgroup_size < len)
            //{
              /*  
                int cli = 5;

                int baa = local_id * _terms;

                for(int j = 0; j < cli * _terms; ++j)
                {
                    local_mem[j] = global_mem[baa + j];//global_id + j];                    
                    out << "local_mem " << local_mem[j] << " lid=" << local_id << " gid=" << global_id << "\r\n";
                }
                */
                //int group = index / _terms;
                //int group_idx = index % _terms; 
                //int moo = sg.get_local_id()[0];
                //local_mem[local_id] = global_mem[(global_id * wgroup_size) + moo];
                //local_mem[local_id] = global_mem[2 * global_id] + global_mem[2 * global_id + 1];
                //out << "local_mem " << local_mem[local_id] << " " << local_id << " " << global_id << "\r\n";
            //}

            local_mem[local_id] = global_mem[global_id];
            local_mem[local_id + 1] = global_mem[global_id + 1];
            out << local_mem[local_id] << "\r\n";

            item.barrier(sycl::access::fence_space::local_space);
out << "done!\r\n";
            for (size_t stride = 1; stride < wgroup_size; stride *= 2) 
            {
                auto idx = 2 * stride * local_id;
                if (idx < wgroup_size)
                {
                    local_mem[idx] = local_mem[idx] + local_mem[idx + stride];
                }

                item.barrier(sycl::access::fence_space::local_space);
            }

            if (local_id == 0) 
            {
                out << "pos=" << item.get_group_linear_id() << " setting " << local_mem[0] << "\r\n";
                global_mem[item.get_group_linear_id()] = local_mem[0];
            }
        });
    });
// ***

outputarb(deviceValues, clients * terms);
    //outputarb(deviceIsFront, clients);
}

void organisation::parallel::front::run4(::parallel::queue *q)
{
    const size_t length = clients;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    qt.memcpy(deviceValues, hostValues, sizeof(float) * terms * clients).wait();
    qt.memset(deviceIsFront, 0, sizeof(int) * clients).wait();

    const int TERMS = 2;
    const int CLIENTS = 30;

    qt.submit([&] (sycl::handler& cgh) 
    {

        auto _values = deviceValues;
        auto _isFront = deviceIsFront;

        auto _terms = terms;
        auto _clients = clients;

        sycl::stream out(65536, 256, cgh);

const size_t GROUPSIZE = 16;
const size_t MAPWIDTH = 160;
const size_t MAPHEIGHT = 160;
        //sycl::range<2> groupSize{GROUPSIZE + 2, GROUPSIZE + 2};
        //sycl::range<2> numGroups{size_t(((MAPWIDTH - 1) / GROUPSIZE) + 1),
          //                          size_t(((MAPWIDTH - 1) / GROUPSIZE) + 1)};
        sycl::range<2> groupSize { (size_t)3,(size_t)terms + 2 };//{ (size_t)1, (size_t)terms };
        sycl::range<2> numGroups { (size_t)1, (size_t)clients };
        
        //sycl::range<1> groupSize { (size_t)terms };
        //sycl::range<1> numGroups { (size_t)clients };

        cgh.parallel_for_work_group<class LocalKernel>(numGroups, groupSize, [=](sycl::group<2> group) 
        {
            float local[TERMS + 2][CLIENTS] = {{ 0 }};

            group.parallel_for_work_item([&](sycl::h_item<2> item)
            {
                //auto sg = item.get_sub_group();

                //out << "local range a " << item.get_local_range()[0] << "\r\n";
                //out << "local range b " << item.get_local_range()[1] << "\r\n";
                //out << "global " << item.get_global_id(0) << "\r\n";
                int local_x = item.get_local_id()[0];
                int local_y = item.get_local_id()[1];

                sycl::id<2> groupId = group.get_group_id();
               // local[groupId[0] * groupId[1]] = _values[groupId[0] * groupId[1]];
               // out << "gid[0]=" << groupId[0] << " gid[1]=" << groupId[1] << "\r\n";
                //if(local_x != 0)
                 out << "lcx=" << local_x << " lcy=" << local_y << "\r\n";

                int x1 = local_x;
                int y1 = (groupId[1] * _terms) + local_y;

                int source = _values[(y1 * _terms) + x1];//_values[((groupId[1] + local_y)  * _terms) + groupId[0] + local_x]; //* groupId[0] + local_x][groupId[1] + local_y];//_values[(groupId[1] * _terms) + local_x];
                out << "source=" << source << " " << (groupId[1] * _terms) + local_x << "\r\n";
            });

        });
    }).wait();

outputarb(deviceValues, clients * terms);
    //outputarb(deviceIsFront, clients);
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