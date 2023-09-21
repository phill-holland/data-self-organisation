#include "parallel/program.hpp"

void organisation::parallel::program::reset(::parallel::device &dev, parameters settings, int clients)
{
    init = false; cleanup();

    this->dev = &dev;
    this->params = settings;

    sycl::queue q = ::parallel::queue(dev).get();

    this->clients = clients;
    this->length = settings.size() * clients;

    deviceValues = sycl::malloc_device<int>(length, q);
    if(deviceValues == NULL) return;
    
    deviceInGates = sycl::malloc_device<int>(settings.size() * settings.in * clients, q);
    if(deviceInGates == NULL) return;

    deviceOutGates = sycl::malloc_device<int>(settings.size() * settings.in * settings.out * clients, q);
    if(deviceOutGates == NULL) return;

    deviceMagnitudes = sycl::malloc_device<int>(settings.size() * settings.in * settings.out * clients, q);
    if(deviceMagnitudes == NULL) return;

    // ***

    hostValues = sycl::malloc_host<int>(settings.size() * HOST_BUFFER, q);
    if(hostValues == NULL) return;
    
    hostInGates = sycl::malloc_host<int>(HOST_BUFFER * settings.size() * settings.in, q);
    if(hostInGates == NULL) return;

    hostOutGates = sycl::malloc_host<int>(HOST_BUFFER * settings.size() * settings.in * settings.out, q);
    if(hostOutGates == NULL) return;

    hostMagnitudes = sycl::malloc_host<int>(HOST_BUFFER * settings.size() * settings.in * settings.out, q);
    if(hostMagnitudes == NULL) return;

    // ***

    deviceOutput = sycl::malloc_device<int>(length, q);
    if(deviceOutput == NULL) return;

    deviceOutputEndPtr = sycl::malloc_device<int>(clients, q);
    if(deviceOutputEndPtr == NULL) return;

    deviceReadPositionsA = sycl::malloc_device<sycl::float4>(length, q);
    if(deviceReadPositionsA == NULL) return;

    deviceReadPositionsB = sycl::malloc_device<sycl::float4>(length, q);
    if(deviceReadPositionsB == NULL) return;

    deviceReadPositionsEndPtr = sycl::malloc_device<int>(clients, q);
    if(deviceReadPositionsEndPtr == NULL) return;

    deviceSourceReadPositions = sycl::malloc_device<sycl::float4>(clients, q);
    if(deviceSourceReadPositions == NULL) return;

    hostSourceReadPositions = sycl::malloc_host<sycl::float4>(clients, q);
    if(hostSourceReadPositions == NULL) return;

    hostOutput = sycl::malloc_host<int>(length, q);
    if(hostOutput == NULL) return;

    hostOutputEndPtr = sycl::malloc_host<int>(clients, q);
    if(hostOutputEndPtr == NULL) return;

    init = true;
}

void organisation::parallel::program::clear(::parallel::queue *q)
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);

    qt.memset(deviceOutputEndPtr, 0, sizeof(int) * clients).wait();
    qt.memset(deviceReadPositionsEndPtr, 0, sizeof(int) * clients).wait();
    
    qt.memset(deviceValues, -1, sizeof(int) * length).wait();
    qt.memset(deviceOutput, 0, sizeof(int) * length).wait();
    qt.memset(deviceReadPositionsA, 0, sizeof(sycl::float4) * length).wait();
    qt.memset(deviceReadPositionsB, 0, sizeof(sycl::float4) * length).wait();
    
    qt.memset(deviceInGates, -1, sizeof(int) * params.size() * params.in * clients);
    qt.memset(deviceOutGates, -1, sizeof(int) * params.size() * params.in * params.out * clients);    
    qt.memset(deviceMagnitudes, -1, sizeof(int) * params.size() * params.in * params.out * clients);    
}

void organisation::parallel::program::run(::parallel::queue *q)
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    sycl::float4 *source = deviceReadPositionsA;
    sycl::float4 *destination = deviceReadPositionsB;

    qt.memset(deviceOutputEndPtr, 0, sizeof(int) * clients).wait();

    int iterations = 0;
    while(iterations++ < ITERATIONS)
    {
        //std::cout << "LOOPPP\r\n";

        qt.memset(destination, 0, sizeof(sycl::float4) * length).wait();
        qt.memset(deviceReadPositionsEndPtr, 0 , sizeof(int) * clients).wait();

        qt.submit([&](auto &h) 
        {        
            auto _values = deviceValues;
            auto _inGates = deviceInGates;
            auto _outGates = deviceOutGates;
            auto _magnitude = deviceMagnitudes;
            auto _output = deviceOutput;
            auto _outputEndPtr = deviceOutputEndPtr;
            auto _readPositionsSource = source;
            auto _readPositionsDest = destination;
            auto _readPositionsEndPtr = deviceReadPositionsEndPtr;
            
            auto _width = params.width;
            auto _height = params.height;
            auto _depth = params.depth;

            auto _in = params.in;
            auto _out = params.out;

            auto _stride = params.size();
            auto _length = length;

//sycl::stream out(4096, 1024, h);

            h.parallel_for(num_items, [=](auto i) 
            {
                if((_readPositionsSource[i].x() != 0)||(_readPositionsSource[i].y() != 0)||(_readPositionsSource[i].z()!= 0))
                {                 
                    sycl::float4 current = _readPositionsSource[i];               

                    int client = i / _stride;
                    //out << "\r\nclient " << client << " " << _readPositionsEndPtr[client] << "\r\n";
                    //out << "x,y,z " << current.x() << " " << current.y() << " " << current.z() << " " << current.w() << "\r\n";

                    int index_moo = ((current.z() * _width * _height) + (current.y() * _width) + current.x());
                    int index = ((current.z() * _width * _height) + (current.y() * _width) + current.x()) + (client * _stride);
//out << "index " << index << "\r\n";
/*
                    int value = _values[index];
                    if(value >= 0) 
                    {
                        int vv = ar.fetch_add(1);
                        if(vv < _stride) 
                            _output[vv + (client * _stride)] = value;
                        out << "value " << value << " cli " << client << "\r\n";
                    }
*/
                    //(((index * length) + i) * params.in) + in_idx
                    //(((client * _stride) + i) * _in) + in_idx
                    //int inIndex = (_in * client);
                    //int inIndex = (((client * _stride) + index) * _in);
                    //int inIndex = (index * length * params.in) + (i * params.in);
                    int inIndex = (client * _stride * _in) + (index_moo * _in);
                   // out << "inIndex " << inIndex << "\r\n";
                    for(int x = 0; x < _in; ++x)
                    {                        
                        if(_inGates[inIndex + x] >= 0)
                        {                            
                            if(_inGates[inIndex + x] == current.w())
                            {
                                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                sycl::memory_scope::device, 
                                sycl::access::address_space::ext_intel_global_device_space> ar(_outputEndPtr[client]);

                                int value = _values[index];
                                if(value >= 0) 
                                {
                                    int vv = ar.fetch_add(1);
                                    if(vv < _stride) 
                                        _output[vv + (client * _stride)] = value;
                                    //out << "value " << value << " cli " << client << "\r\n";
                                }
                                // ********

                                //out << "ingate[] " << _inGates[inIndex + x] << " w " << current.w() << " _in " << _in << "\r\n"; 
                                for(int y = 0; y < _out; ++y)
                                {                            
                                    //int s = (((client * _stride) + index_moo) * _out * _in);// + in_idx
                                    int s = (client * _stride * _in * _out);
                                    s += (index_moo * _out * _in);
                                    s += (y * _in) + x;
                                    int outIndex = s;

                                   // out << "outgate[] " << _outGates[outIndex] << " outIndex " << outIndex << "\r\n";
                                    //int outIndex = (_in * y) +  x;
                                    if(_outGates[outIndex] >= 0)
                                    {                                        
                                        int outTemp = _outGates[outIndex];
                                        float magnitudeTemp = (float)_magnitude[outIndex];
                                        
                                       // out << "outgate[] " << y << " w=" << outTemp << " m=" << magnitudeTemp << " " << _out << "\r\n";
                                        //out << "mag " << _magnitude[outIndex] << "\r\n";
                                        if(magnitudeTemp > 0)
                                        {
                    
                                            int r = outTemp % 9;//div(index, 9);
                                            float z1 = (float)((outTemp / 9) - 1);//(float)r.quot - 1L;

                                            int j = r % 3;//div(r.rem, 3);
                                            float y1 = (float)((r / 3) - 1);//(float)j.quot - 1L;
                                            float x1 = (float)(j - 1);//(float)j.rem - 1L;

                                            float z2 = (z1 * magnitudeTemp) + current.z();
                                            float y2 = (y1 * magnitudeTemp) + current.y();
                                            float x2 = (x1 * magnitudeTemp) + current.x();

                                            //out << "out x1,y1,z1 " << (x1 * magnitudeTemp) << "," << (y1 * magnitudeTemp) << "," << (z1 * magnitudeTemp) << "\r\n";
                                            //out << "out x2,y2,z2 " << x2 << "," << y2 << "," << z2 << " " << outTemp << "\r\n";

                                            if((x2 >= 0)&&(x2 < _width)&&(y2 >= 0)&&(y2 < _height)&&(z2 >=0)&&(z2 < _depth))
                                            {
                                                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                                sycl::memory_scope::device, 
                                                sycl::access::address_space::ext_intel_global_device_space> br(_readPositionsEndPtr[client]);

                                                int tx = roundf(-x1) + 1;
                                                int ty = roundf(-y1) + 1;
                                                int tz = roundf(-z1) + 1;

        //out << "out tx,ty,tz " << (x1*-1.0f) << "," << (y1*-1.0f) << "," << (z1*-1.0f) << "\r\n";

                                                float w = (float)((sycl::abs(tz) * (3 * 3)) + (sycl::abs(ty) * 3) + sycl::abs(tx));

                                                int vv = br.fetch_add(1);
                                               // out << "new gate " << w << " out " << vv << "\r\n";
                                                if(vv < _stride)
                                                    _readPositionsDest[vv + (client * _stride)] = { x2,y2,z2,w };
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            });
        }).wait();

//std::cout << "DESTINATIOON\r\n";
//outputarb(destination, length);


        sycl::float4 *temp = destination;
        destination = source;
        source = temp;
    };
}

void organisation::parallel::program::set(std::vector<sycl::float4> positions, ::parallel::queue *q)
{
    if(positions.size() != clients) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)clients};

    memcpy(hostSourceReadPositions, positions.data(), sizeof(sycl::float4) * clients);
    qt.memcpy(deviceSourceReadPositions, hostSourceReadPositions, sizeof(sycl::float4) * clients).wait();

     qt.submit([&](auto &h) 
    {        
        auto _readPositionsSource = deviceSourceReadPositions;
        auto _readPositions = deviceReadPositionsA;
        auto _readPositionsEndPtr = deviceReadPositionsEndPtr;
        
        auto _dimLength = params.size();
        auto _length = length;

        h.parallel_for(num_items, [=](auto i) 
        {
            _readPositions[i * _dimLength] = _readPositionsSource[i];
            _readPositionsEndPtr[i] = 1;
        });
    }).wait();

    //outputarb(deviceReadPositionsA,length);
    //outputarb(deviceReadPositionsEndPtr,clients);
}

std::vector<organisation::parallel::output> organisation::parallel::program::get(::parallel::queue *q)
{
    std::vector<output> results(clients);

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);

//std::cout << "outputs\r\n";
//outputarb(deviceOutput, length);
//outputarb(deviceOutputEndPtr, clients);

    qt.memcpy(hostOutput, deviceOutput, sizeof(int) * length).wait();
    qt.memcpy(hostOutputEndPtr, deviceOutputEndPtr, sizeof(int) * clients).wait();

    for(int i = 0; i < length; ++i)
    {
        int client = i / params.size();;
        int index = i % params.size();

        if(index < hostOutputEndPtr[client])
        {
            results[client].values.push_back(hostOutput[i]);
        }
    }

    return results;
}

void organisation::parallel::program::copy(::organisation::schema **source, int source_size, ::parallel::queue *q)
{
    memset(hostValues, -1, sizeof(int) * params.size() * HOST_BUFFER);
    memset(hostInGates, -1, sizeof(int) * params.size() * params.in * HOST_BUFFER);
    memset(hostOutGates, -1, sizeof(int) * params.size() * params.in * params.out * HOST_BUFFER);
    memset(hostMagnitudes, -1, sizeof(int) * params.size() * params.in * params.out * HOST_BUFFER);

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)clients};

    int dest_index = 0;
    int index = 0;
    //for(std::vector<::organisation::program>::iterator it = source.begin(); it != source.end(); ++it)
    for(int source_index = 0; source_index < source_size; ++source_index)
    {
        organisation::program *prog = &source[source_index]->prog;

        //int length = it->length;
        int length = prog->length;

        //std::cout << "PROGRAM " << length << "\r\n";

        if(length != params.size()) 
            break;


        for(int i = 0; i < length; ++i)
        {
            //hostValues[0] = 1;
            //if(it->cells.at(i).value >=0) std::cout << "hostValues " << ((index * length) + i) << " " << it->cells.at(i).value << "\r\n";
            hostValues[(index * length) + i] = prog->cells.at(i).value;

            std::vector<int> in = prog->cells.at(i).pull();
            int in_idx = 0;
            for(std::vector<int>::iterator jt = in.begin(); jt < in.end(); ++jt)
            {
                if(in_idx < params.in)
                {                    
                    int inIndex = (index * length * params.in) + (i * params.in);
                    // (((index * length) + i) * params.in)
                    hostInGates[inIndex + in_idx] = *jt;
                    //hostInGates[in_idx + (index * params.in)] = *jt;
//std::cout << "in_idx " << in_idx << " jt=" << *jt << " " << (inIndex + in_idx) << "\r\n";
                    std::vector<int> out = prog->cells.at(i).pull(*jt);
                    int out_idx = 0;
                    for(std::vector<int>::iterator ot = out.begin(); ot < out.end(); ++ot)
                    {
                        if(out_idx < params.out)
                        {
                            int s = (index * length * params.in * params.out);
                            s += (i * params.out * params.in);
                            s += (out_idx * params.in) + in_idx;
                            
                            //int outIndex = s;

                            //int s = (((index * length) + i) * params.out * params.in);// + in_idx
                            //s += (out_idx * params.in) + in_idx;
                            //int s = in_idx + (out_idx * params.in) + (index * params.in * params.out);
                            hostOutGates[s] = *ot;
                            hostMagnitudes[s] = prog->cells.at(i).get(*jt,*ot).magnitude;

//std::cout << "out_idx " << out_idx << " ot=" << *ot << " " << s << "\r\n";
                            // need new device magnitude buffer, of params.in * params.out * clients
                        }

                        ++out_idx;
                    }
                }
                ++in_idx;
            }
        }

        ++index;
        if(index >= HOST_BUFFER)
        {
            //std::cout << "dest_index " << dest_index << " length " << length << "\r\n\r\n";
            // copy from host to device
            qt.memcpy(&deviceValues[dest_index * length], hostValues, sizeof(int) * length * index).wait();
            qt.memcpy(&deviceInGates[dest_index * params.in * length], hostInGates, sizeof(int) * params.in * length * index).wait();
            qt.memcpy(&deviceOutGates[dest_index * params.in * params.out * length], hostOutGates, sizeof(int) * params.in * params.out * length * index).wait();
            qt.memcpy(&deviceMagnitudes[dest_index * params.in * params.out * length], hostMagnitudes, sizeof(int) * params.in * params.out * length * index).wait();

            memset(hostValues, -1, sizeof(int) * params.size() * HOST_BUFFER);
            memset(hostInGates, -1, sizeof(int) * params.size() * params.in * HOST_BUFFER);
            memset(hostOutGates, -1, sizeof(int) * params.size() * params.in * params.out * HOST_BUFFER);
            memset(hostMagnitudes, -1, sizeof(int) * params.size() * params.in * params.out * HOST_BUFFER);

            dest_index += HOST_BUFFER;//index;
            index = 0;            
        }
    }

    if(index > 0)
    {
        // copy from host to device
        int length = params.size();

        qt.memcpy(&deviceValues[dest_index * length], hostValues, sizeof(int) * length * index).wait();
        qt.memcpy(&deviceInGates[dest_index * params.in * length], hostInGates, sizeof(int) * params.in * length * index).wait();
        qt.memcpy(&deviceOutGates[dest_index * params.in * params.out * length], hostOutGates, sizeof(int) * params.in * params.out * length * index).wait();
        qt.memcpy(&deviceMagnitudes[dest_index * params.in * params.out * length], hostMagnitudes, sizeof(int) * params.in * params.out * length * index).wait();
        
        //outputarb(deviceValues, this->length);
        //std::cout << "in gates\r\n";
        //outputarb(deviceInGates, clients * params.size() * params.in);
        //std::cout << "out gates\r\n";
        //outputarb(deviceOutGates, clients * params.size() * params.in * params.out);
    }
}

void organisation::parallel::program::outputarb(int *source, int length)
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
			result += std::to_string(i);//string::fromInt(i));
			result += std::string("]");
			result += std::to_string(temp[i]);//string::fromInt(temp[i]));
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::program::outputarb(sycl::float4 *source, int length)
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
			result += std::to_string(i);//string::fromInt(i));
			result += std::string("]");
			result += std::to_string(temp[i].x());//std::string::fromFloat(temp[i].x()));
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


void organisation::parallel::program::makeNull()
{
    dev = NULL;

    deviceValues = NULL;
    deviceInGates = NULL;
    deviceOutGates = NULL;
    deviceMagnitudes = NULL;
    
    hostValues = NULL;
    hostInGates = NULL;
    hostOutGates = NULL;
    hostMagnitudes = NULL;

    deviceOutput = NULL;
    deviceOutputEndPtr = NULL;

    deviceReadPositionsA = NULL;
    deviceReadPositionsB = NULL;
    deviceReadPositionsEndPtr = NULL;   
    
    deviceSourceReadPositions = NULL;
    hostSourceReadPositions = NULL;

    hostOutput = NULL;
    hostOutputEndPtr = NULL; 
}

void organisation::parallel::program::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();

        if (hostOutputEndPtr != NULL) sycl::free(hostOutputEndPtr, q);
        if (hostOutput != NULL) sycl::free(hostOutput, q);

        if (hostSourceReadPositions != NULL) sycl::free(hostSourceReadPositions, q);
        if (deviceSourceReadPositions != NULL) sycl::free(deviceSourceReadPositions, q);

        if (deviceReadPositionsEndPtr != NULL) sycl::free(deviceReadPositionsEndPtr, q);
        if (deviceReadPositionsA != NULL) sycl::free(deviceReadPositionsA, q);
        if (deviceReadPositionsB != NULL) sycl::free(deviceReadPositionsB, q);

        if (deviceOutputEndPtr != NULL) sycl::free(deviceOutputEndPtr, q);
        if (deviceOutput != NULL) sycl::free(deviceOutput, q);

        if (hostMagnitudes != NULL) sycl::free(hostMagnitudes, q);
        if (hostOutGates != NULL) sycl::free(hostOutGates, q);
        if (hostInGates != NULL) sycl::free(hostInGates, q);
        if (hostValues != NULL) sycl::free(hostValues, q);

        if (deviceMagnitudes != NULL) sycl::free(deviceMagnitudes, q);
        if (deviceOutGates != NULL) sycl::free(deviceOutGates, q);
        if (deviceInGates != NULL) sycl::free(deviceInGates, q);
        if (deviceValues != NULL) sycl::free(deviceValues, q);
    }
}