#include <iostream>
#include <fstream>
#include <cstring>
#include <Python.h>
#include "chartype.hpp"
#include "transducer.hpp"
#include "icu.hpp"

extern "C" 
{
    Transducer* newTransducer()
    {
        return new Transducer();
    }

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

    PyObject* screen(const char* s)
    {
        auto ucs = UnicodeString::fromUTF8(s);
        auto st = convertUnicode(ucs);
        auto screened = screen(st);
        std::string str_;
        auto str = convertFromStringType(screened).toUTF8String(str_);
        return PyString_FromString(str_.c_str());
    }

    Transducer* transducerFromAligmentModel(const char* filename)
    {
        std::ifstream fin(filename);
        auto t = Transducer::fromAlignmentModel(fin);
        return new Transducer(t);
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
            std::string str_;
            auto str = convertFromStringType(edge -> io.toString()).toUTF8String(str_);
            auto edge_tuple = Py_BuildValue("(Oi)", PyString_FromString(str_.c_str()), edge -> weight);
            PyList_Append(list, edge_tuple);
        }

        return list;
    }

    Transducer* transducerComposition(Transducer* t1, Transducer* t2)
    {
        return new Transducer(t1 -> composition(*t2));
    }

	bool isTransducerEmpty(Transducer* t)
	{
		return t->isEmpty();
	}

	void minWay(Transducer* t)
	{
		t->minWay();
	}
}
