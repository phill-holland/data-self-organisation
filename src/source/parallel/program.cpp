#include "parallel/program.hpp"

void organisation::parallel::program::reset(::parallel::device &dev, dimensions dim, int clients)
{
    init = false; cleanup();

    this->dev = &dev;

    sycl::queue q = ::parallel::queue(dev).get();

    int length = dim.size() * clients;

    deviceValues = sycl::malloc_device<int>(length, q);
    if(deviceValues == NULL) return;
    
    deviceGates = sycl::malloc_device<int>(length, q);
    if(deviceGates == NULL) return;

    deviceClient = sycl::malloc_device<int>(length, q);
    if(deviceClient == NULL) return;

    init =true;
}

void organisation::parallel::program::run(::parallel::queue *q)
{
    /*
        sycl::queue& qt = ::parallel::queue::get_queue(*dev, q);
    sycl::range num_items{(size_t)global.length};

    qt.memset(deviceInsertWritePtr, 0, sizeof(int)).wait();

    qt.submit([&](auto &h) 
    {        
        auto _position = devicePosition;
        auto _nextDirection = deviceNextDirection;
        auto _nextHalfPosition = deviceNextHalfPosition;
        auto _directionIndices = deviceDirectionIndices;
        auto _movementIndices = deviceMovementIndices;
        auto _lifetime = deviceLifetime;
        auto _age = deviceAge;
        auto _dataHash = deviceDataHash;
        auto _cost = deviceCost;
        auto _collisionDirectionState = deviceCollisionDirectionState;
        auto _readBuffer = deviceReadBuffer;
        auto _client = deviceClient;
        
        h.parallel_for(num_items, [=](auto i) 
        { 
            _position[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
            _nextDirection[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
            _nextHalfPosition[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
            
            _directionIndices[i] = 0;
            _movementIndices[i] = 0;
            _lifetime[i] = 0;

            _age[i] = 0;
            _dataHash[i] = 0;
            _cost[i] = 0;

            _collisionDirectionState[i] = { 0, 0 };
            _readBuffer[i] = { 0, 0, 0, 0 };

            _client[i] = { 0, 0, 0, 0 };
        });
    }).wait();
*/
}

void organisation::parallel::program::makeNull()
{
    deviceValues = NULL;
    deviceGates = NULL;
    deviceClient = NULL;
}

void organisation::parallel::program::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();

        if (deviceClient != NULL) sycl::free(deviceClient, q);
        if (deviceGates != NULL) sycl::free(deviceGates, q);
        if (deviceValues != NULL) sycl::free(deviceValues, q);
    }
}