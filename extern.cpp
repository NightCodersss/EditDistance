#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <Python.h>
#include "chartype.hpp"
#include "transducer.hpp"
#include "icu.hpp"
#include "transduceroptimizer.hpp"

extern "C" 
{
    Transducer* newTransducer()
    {
        return new Transducer();
    }

    Transducer* transducerFromRegexp(const char* regexp)
    {
        auto ucs = UnicodeString::fromUTF8(regexp);

        string_type s = convertUnicode(ucs);

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
        if ( !fin )
            throw std::logic_error(std::string("Can't open file ") + filename);
        auto t = Transducer::fromAlignmentModel(fin);
        return new Transducer(std::move(t));
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

        auto list = PyList_New(path.path.size() * 2);
        auto i = 0;
        for ( const auto& edge : path.path )
        {
            std::string str_;
            auto str = convertFromStringType(edge -> io.toString()).toUTF8String(str_);
            auto weight = edge -> weight;
            PyList_SetItem(list, i, PyString_FromString(str_.c_str()));
            PyList_SetItem(list, i + 1, PyInt_FromLong(weight));
            i += 2;
        }

        return list;
    }

    Transducer* transducerComposition(Transducer* t1, Transducer* t2)
    {
        auto t = new Transducer(t1 -> composition(std::move(*t2)));
        return t;
    }

	bool isTransducerEmpty(Transducer* t)
	{
		return t->isEmpty();
	}

	void minWay(Transducer* t)
	{
		t->minWay();
	}

    void release(Transducer* t)
    {
        delete t;
    }

    void optimizeTransducer(Transducer* t)
    {
        TransducerOptimizer to(*t);
        to.optimize();        
    }
}
