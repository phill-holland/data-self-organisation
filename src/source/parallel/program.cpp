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
    
    deviceInGates = sycl::malloc_device<int>(clients * settings.in, q);
    if(deviceInGates == NULL) return;

    deviceOutGates = sycl::malloc_device<int>(clients * settings.in * settings.out, q);
    if(deviceOutGates == NULL) return;

    //deviceClient = sycl::malloc_device<int>(length, q);
    //if(deviceClient == NULL) return;

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

    deviceSourceReadPositions = sycl::malloc_device<int>(clients, q);
    if(deviceSourceReadPositions == NULL) return;

    hostSourceReadPositions = sycl::malloc_host<int>(clients, q);
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
    //sycl::range num_items{(size_t)length};

    qt.memset(deviceValues, 0, sizeof(int) * length);
    qt.memset(deviceOutput, 0, sizeof(int) * length);
    qt.memset(deviceOutputEndPtr, 0, sizeof(int) * clients);
    qt.memset(deviceReadPositionsA, 0, sizeof(sycl::float4) * length);
    qt.memset(deviceReadPositionsB, 0, sizeof(sycl::float4) * length);
    qt.memset(deviceReadPositionsEndPtr, 0, sizeof(int) * clients).wait();


    // clear deviceValues
    // clear deviceReadPositions
    // clear devicereadPositionsEndPtr
}

void organisation::parallel::program::run(::parallel::queue *q)
{
    // positions.w == inital set needs to be to vector encoding index number (from source intput vector)
    // gates copied into system, empty ones need to be -1
    // include new single int device memory, for counting number of outputs

// copy function
// creates N amount of programs, in pinned memory, copy in chunks from source

    // positions, in order of client index
    // deviceReads per client, (length * client)
    // stores current read position, per loop
    // devicereadEndPtr = atomic structure, stores the end pointer per client 


    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)length};

    sycl::float4 *source = deviceReadPositionsA;
    sycl::float4 *destination = deviceReadPositionsB;

    // need special fill function!!!
    //qt.memcpy(source, positions.data(), sizeof(sycl::float4) * length).wait();
// then fill readPositionsEndPtr to 1 as well!!!!

// loop here -- until nothing in destination buffer (another single int atomic buffer??)
    int iterations = 0;
    while(iterations++ < ITERATIONS)
    {
        qt.memset(destination, 0, sizeof(sycl::float4) * length).wait();
        qt.memset(deviceReadPositionsEndPtr, 0 , sizeof(int) * clients).wait();

    // swap buffer at end of loop (inside loop)

        qt.submit([&](auto &h) 
        {        
            auto _values = deviceValues;
            auto _inGates = deviceInGates;
            auto _outGates = deviceOutGates;
            //auto _client = deviceClient;
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

            auto _length = length;

    // READ_POSITION.W IS THE IN GATE INDEX
    // EMPTY GATES IN OUT MARKED AS -1
            h.parallel_for(num_items, [=](auto i) 
            {
                if((_readPositionsSource[i].x() != 0)||(_readPositionsSource[i].y() != 0)||(_readPositionsSource[i].z()!= 0))
                {                 
                    sycl::float4 current = _readPositionsSource[i];               
                    //int client = _client[i];
                    int client = i % _length;

                    cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                sycl::memory_scope::device, 
                                sycl::access::address_space::ext_intel_global_device_space> ar(_outputEndPtr[client]);

                    int index = ((current.z() * _width * _height) + (current.y() * _width) + current.x()) * client;

                    int value = _values[index];
                    if(value >= 0) _output[ar.fetch_add(1)] = value;

                    int inIndex = (_in * client);
                    for(int x = 0; x < _in; ++x)
                    {
                        if(_inGates[inIndex + x] == current.w())
                        {
                            for(int y = 0; y < _out; ++y)
                            {                            
                                int outIndex = (_in * y) +  x;
                                if(_outGates[outIndex] >= 0)
                                {
                                    int r = index % 9;//div(index, 9);
                                    float z1 = (float)((index / 9) - 1);//(float)r.quot - 1L;

                                    int j = r % 3;//div(r.rem, 3);
                                    float y1 = (float)((r / 3) - 1);//(float)j.quot - 1L;
                                    float x1 = (float)(j - 1);//(float)j.rem - 1L;

                                    float z2 = z1 + current.z();
                                    float y2 = y1 + current.y();
                                    float x2 = x1 + current.x();

                                    if((x1 >= 0)&&(x1 < _width)&&(y1 >= 0)&&(y1 < _height)&&(z1 >=0)&&(z1 < _depth))
                                    {
                                        cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                        sycl::memory_scope::device, 
                                        sycl::access::address_space::ext_intel_global_device_space> br(_readPositionsEndPtr[client]);

                                        int tx = roundf(-z1) + 1;
                                        int ty = roundf(-y1) + 1;
                                        int tz = roundf(-z1) + 1;

                                        //if ((tx < 0L) || (tx > 2)) return 0L;
                                        //if ((ty < 0L) || (ty > 2)) return 0L;
                                        //if ((tz < 0) || (tz > 2)) return 0L;
    //div_t moo = sycl::div(1,2);
                                        float w = (float)((sycl::abs(tz) * (3 * 3)) + (sycl::abs(ty) * 3) + sycl::abs(tx));
                                        _readPositionsDest[br.fetch_add(1)] = { z2,y2,x2,w };

                                        // insert new position in output read buffer
                                        // inverse vector, and then reencode to single value for position.w
                                    }
                                    //w = 0.0f;

                                    
                                }
                            }
                        }
                    }
                    //int out_index 
                    // if value != 0 output
                    // find out gates
                    // generate new readPositions
                }

                // need to dump out new read positions into another tempArray ..??
                /*
                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                sycl::memory_scope::device, 
                                sycl::access::address_space::ext_intel_global_device_space> ar(_insertWritePtr[0]);
                                */

            });
        }).wait();

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

    memcpy(hostSourceReadPositions, positions.data(), sizeof(int) * clients);
    qt.memcpy(deviceSourceReadPositions, deviceSourceReadPositions, sizeof(int) * clients).wait();

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
            _readPositionsEndPtr[i * _dimLength] = 1;
        });
    }).wait();
}

std::vector<organisation::parallel::output> organisation::parallel::program::get(::parallel::queue *q)
{
    std::vector<output> results(clients);

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)clients};

    qt.memcpy(hostOutput, deviceOutput, sizeof(int) * length).wait();
    qt.memcpy(hostOutputEndPtr, deviceOutputEndPtr, sizeof(int) * clients).wait();

    for(int i = 0; i < length; ++i)
    {
        div_t result = div(i, params.size());

        int client = result.rem;
        int index = result.quot;

        if(index < hostOutputEndPtr[client])
        {
            results[client].values.push_back(hostOutput[i]);
        }
    }

    return results;
}

void organisation::parallel::program::makeNull()
{
    deviceValues = NULL;
    deviceInGates = NULL;
    deviceOutGates = NULL;
    //deviceClient = NULL;

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
                
       // if (deviceClient != NULL) sycl::free(deviceClient, q);
        if (deviceOutGates != NULL) sycl::free(deviceOutGates, q);
        if (deviceInGates != NULL) sycl::free(deviceInGates, q);
        if (deviceValues != NULL) sycl::free(deviceValues, q);
    }
}