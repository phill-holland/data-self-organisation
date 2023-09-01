#ifndef _ORGANISATION_VECTOR
#define _ORGANISATION_VECTOR

namespace organisation
{    
    class vector
    {  
    public:
        int x,y,z,w;

    public:
        vector() { x = y = z = 0; w = 1; }
        vector(int a, int b, int c) { x = a; y = b; z = c; w = 1; }
        vector(int a, int b, int c, int d) { x = a; y = b; z = c; w = d; }

        vector inverse()
        {
            return { x * -1, y * -1, z * -1, w };
        }

        vector normalise()
        {
            return { x / w, y / w, z / w, w };
        }
    };
};

#endif