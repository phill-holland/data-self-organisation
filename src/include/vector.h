#ifndef _ORGANISATION_VECTOR
#define _ORGANISATION_VECTOR

namespace organisation
{    
    class vector
    {  
    public:
        int x,y,z;

    public:
        vector inverse()
        {
            return { x * -1, y * -1, z * -1 };
        }
    };
};

#endif