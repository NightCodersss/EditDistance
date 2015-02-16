#include <iostream>
#include <fstream>
#include <cstring>
#include "chartype.hpp"
#include "transducer.hpp"

extern "C" 
{
    Transducer* transducerFromRegexp(const char* regexp)
    {
        string_type s(strlen(regexp));
        for ( int i = 0; i < s.size(); ++i )
            s[i] = regexp[i];
        return new Transducer(Transducer::fromRegexp(s));
    }

    void readTransducerFromFile(Transducer* t, const char* filename)
    {
        std::ifstream fin(filename);
        t -> readFromFile(fin);
    }

    void visualizeTransducer(Transducer* t)
    {
        t -> visualize(std::cout);
    }
}
