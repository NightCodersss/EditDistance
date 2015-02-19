#include <iostream>
#include <fstream>
#include <cstring>
#include <Python.h>
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

    void resetTransducerMinPaths(Transducer* t)
    {
        t -> resetMinPaths();
    }

    PyObject* getTransducerNextMinPath(Transducer* t)
    {
        auto path = t -> getNextMinPath();        

        auto list = PyList_New(0);
        for ( const auto& edge : path.path )
        {
            auto str = edge -> io.toString();
            auto edge_tuple = Py_BuildValue("(Oi)", PyString_FromString(str.c_str()), edge -> weight);
            PyList_Append(list, edge_tuple);
        }

        return list;
    }
}
