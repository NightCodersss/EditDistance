#ifndef _REGEXPPARSER_HPP_
#define _REGEXPPARSER_HPP_

#include <string>
#include <boost/variant.hpp>
#include "transducer.hpp"
#include "intervalunion.hpp"

class RegexpParser
{
public:

    RegexpParser() = default;
    RegexpParser(string_type regexp);

    Transducer parse(string_type regexp);
    IOPart parseIOPart();

private:

    void failWith(std::string error);

    int parseInt();

    Transducer parseOr();    
    Transducer parseKlenee();    
    Transducer parseConcat();
    Transducer parseBlock();

    void consume();
    void match(char_type c);

    IntervalUnion parseInterval();

private:    

    string_type regexp;
    size_t pos;
};

#endif
