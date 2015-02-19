#include <iostream>
#include <fstream>
#include <cstring>
#include "chartype.hpp"
#include "transducer.hpp"
#include "icu.hpp"

extern "C" 
{
    Transducer* transducerFromRegexp(const char* regexp)
    {
        auto ucs = UnicodeString::fromUTF8(regexp);

        std::cout << "UCS = " << ucs << '\n';

        string_type s = convertUnicode(ucs);

        for ( auto i : s )
            std::cout << "Int: " << i << ' ';
        std::cout << '\n';

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
