#include "regexpparser.hpp"
#include <ctype.h>
#include <stdexcept>

Transducer RegexpParser::parse(std::string _regexp)
{
    regexp = _regexp;
    pos = 0;

    Transducer t = parseOr();

    if ( pos != regexp.length() )
        throw std::logic_error("Wrong syntax in regular expression");

    return t;
}

void RegexpParser::consume()
{
    if ( pos < regexp.length() )
    {
        ++pos;
        while ( pos < regexp.length() && isspace(regexp[pos]) )
            ++pos;
    }
}

void RegexpParser::match(char c)
{
    if ( regexp[pos] != c )
        throw std::logic_error("Wrong syntax in regular expression");
    else
        consume();
}

Transducer RegexpParser::parseOr()
{
    Transducer t = parseConcat();

    while ( regexp[pos] == '|' )
    {
        match('|');
        t = Transducer::orTransducer(t, parseConcat());
    }

    return t;
}

Transducer RegexpParser::parseKlenee()
{
    Transducer t;

    if ( regexp[pos] == '(' )
    {
        match('(');
        t = parseOr();
        match(')');
    }
    else
    {
        t = Transducer::matchChar(std::string("") + regexp[pos]);
        consume();
    }



    if ( regexp[pos] == '*' )
    {
        match('*');
        t = Transducer::klenee(t);
    }

    return t;
}

Transducer RegexpParser::parseConcat()
{
    Transducer t = parseKlenee();

    while ( regexp[pos] == '(' || isalnum(regexp[pos]) )
        t = Transducer::concat(t, parseKlenee());

    return t;
}
