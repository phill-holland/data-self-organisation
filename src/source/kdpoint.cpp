#include "kdpoint.h"
#include <limits.h>
#include <string>

void dominance::kdtree::kdpoint::reset(long dimensions)
{
    init = false; cleanup();

    left = NULL; right = NULL;

    this->dimensions = dimensions;

    identity = 0L;

    values.reserve(dimensions);
    min.reserve(dimensions);
    max.reserve(dimensions);

    for(long i=0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }

    init = true;
}

void dominance::kdtree::kdpoint::clear()
{
    left = NULL; right = NULL;

    for(long i=0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }
}

void dominance::kdtree::kdpoint::set(long *points, unsigned long length)
{
    long min = length;
    if(min == 0L) min = dimensions;
    if(min > dimensions) min = dimensions;

    for(long i = 0L; i < min; ++i)
    {
        values[i] = points[i];
    }
}

bool dominance::kdtree::kdpoint::set(long value, unsigned long dimension)
{
    if(dimension >= dimensions) return false;
    if(dimension < 0L) return false;

    values[dimension] = value;

    return true;
}

void dominance::kdtree::kdpoint::set(long value)
{
    for(long i = 0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }
}

long dominance::kdtree::kdpoint::get(unsigned long dimension)
{
    if(dimension >= dimensions) return -1L;
    if(dimension < 0L) return -1L;

    return values[dimension];
}

void dominance::kdtree::kdpoint::copy(const kdpoint &source)
{
    if (dimensions <= source.dimensions)
    {
        long _min = dimensions;
        if(source.dimensions < _min) _min = source.dimensions;

        for(long i=0L; i < _min; ++i)
        {
            values[i] = source.values[i];
            min[i] = source.min[i];
            max[i] = source.max[i];
        }
    
        dimensions = source.dimensions;
        identity = source.identity;
    }
}

bool dominance::kdtree::kdpoint::equals(const kdpoint &source)
{
    if (dimensions < source.dimensions) return false;
    
    long min = dimensions;
    if(source.dimensions < min) min = source.dimensions;

    for(long i=0L; i < min; ++i)
    {
        if(values[i] != source.values[i]) return false;
    }
    
    return true;
}

bool dominance::kdtree::kdpoint::inside(const kdpoint &min, const kdpoint &max)
{
    if(min.dimensions != dimensions) return false;
    if(max.dimensions != dimensions) return false;

    for(long i=0L; i < dimensions; ++i)
    {
        if(values[i] < min.values[i]) return false;
        if(values[i] > max.values[i]) return false;
    }

    return true;
}

bool dominance::kdtree::kdpoint::issame(long value)
{
    for(long i=0L; i < dimensions; ++i)
    {
        if(values[i] != value) return false;
    }

    return true;
}

void dominance::kdtree::kdpoint::makeNull()
{
    
}

void dominance::kdtree::kdpoint::cleanup()
{

}
