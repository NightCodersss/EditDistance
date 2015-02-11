#include "regexpparser.hpp"
#include <ctype.h>
#include <stdexcept>

Transducer RegexpParser::parse(string_type _regexp)
{
    regexp = _regexp;
    pos = 0;

    Transducer t = parseOr();

    if ( pos != regexp.size() )
        throw std::logic_error("Wrong syntax in regular expression");

    return t;
}

void RegexpParser::consume()
{
    if ( pos < regexp.size() )
    {
        ++pos;
        while ( pos < regexp.size() && isspace(regexp[pos]) )
            ++pos;
    }
}

void RegexpParser::match(char_type c)
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

Transducer RegexpParser::parseBlock()
{
    match('[');

    bool complement = regexp[pos] == '^';
    if ( complement )
        match('^');

    IntervalUnion iu(complement, { });

    while ( regexp[pos] != ']' )
    {
        auto c = regexp[pos];
        consume();

        if ( regexp[pos] == '-' )
        {
            match('-');
            auto c2 = regexp[pos];
            consume();

            if ( c2 == ']' )
                throw std::logic_error("Unmatched range");

            if ( c <= c2 )
                iu.addInterval({c, c2});
        }
        else if ( c == '\\' )
        {
            // \[ \] \- \^ \n \r \t \0

            auto c2 = regexp[pos];
            switch ( c2 )
            {
                case 'n': iu.addInterval({'\n', '\n'}); break;
                case 'r': iu.addInterval({'\r', '\r'}); break;
                case 't': iu.addInterval({'\t', '\t'}); break;
                case '0': iu.addInterval({'\0', '\0'}); break;
                case '[': case ']': case '-': case '^': case '\\':
                {
                    iu.addInterval({c2, c2});
                    break;
                }
                default:
                    throw std::logic_error("Unknown symbol \\" + std::to_string(c2)); 
            }
        }
        else
            iu.addInterval({c, c});
    }        

    match(']');

    return Transducer::matchBlock(iu); 
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
    else if ( regexp[pos] == '[' )
    {
        t = parseBlock();
    }
    else
    {
        t = Transducer::matchChar(regexp[pos]);
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
