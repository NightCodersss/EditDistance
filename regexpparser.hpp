#ifndef _REGEXPPARSER_HPP_
#define _REGEXPPARSER_HPP_

#include <string>
#include "transducer.hpp"

class RegexpParser
{
public:

    Transducer parse(std::string regexp);

private:

    Transducer parseOr();    
    Transducer parseKlenee();    
    Transducer parseConcat();

    void consume();
    void match(char c);

private:    

    std::string regexp;
    size_t pos;
};

#endif
