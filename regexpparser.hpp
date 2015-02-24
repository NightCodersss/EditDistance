#ifndef _REGEXPPARSER_HPP_
#define _REGEXPPARSER_HPP_

#include <string>
#include <boost/variant.hpp>
#include "transducer.hpp"
#include "intervalunion.hpp"

class RegexpParser
{
public:

    RegexpParser(string_type regexp);

    Transducer parse(string_type regexp);

private:

    Transducer parseOr();    
    Transducer parseKlenee();    
    Transducer parseConcat();
    Transducer parseBlock();

    void consume();
    void match(char_type c);

    IntervalUnion parseInterval();
    boost::variant<IntervalUnion, char_type> parseIOPart();

private:    

    string_type regexp;
    size_t pos;
};

#endif
