#ifndef _REGEXPPARSER_HPP_
#define _REGEXPPARSER_HPP_

#include <string>
#include "transducer.hpp"

class RegexpParser
{
public:

    Transducer parse(string_type regexp);

private:

    Transducer parseOr();    
    Transducer parseKlenee();    
    Transducer parseConcat();
    Transducer parseBlock();

    void consume();
    void match(char_type c);

private:    

    string_type regexp;
    size_t pos;
};

#endif
