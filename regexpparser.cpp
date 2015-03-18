#include "regexpparser.hpp"
#include <ctype.h>
#include <stdexcept>
#include "icu.hpp"

RegexpParser::RegexpParser(string_type regexp) : regexp(regexp), pos(0) { }

Transducer RegexpParser::parse(string_type _regexp)
{
    regexp = _regexp;
    pos = 0;
    
	if(regexp.size() == 0)
		return Transducer();

    Transducer t = parseOr();

    if ( pos != regexp.size() )
        throw std::logic_error("Wrong syntax in regular expression");

    return t;
}

void RegexpParser::failWith(std::string error)
{
    std::cerr << "Error: failed on regexp " << convertFromStringType(regexp) << '\n';
    throw std::logic_error("Error: " + error + "; at " + std::to_string(pos));
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
	if ( pos >= regexp.size() )
        failWith("Wrong syntax in regular expression: expected something, found end of string");
    if ( regexp[pos] != c )
        failWith("Wrong syntax in regular expression: expected something, found something else");
    else
        consume();
}

int RegexpParser::parseInt()
{
    if ( pos < regexp.size() && '0' <= regexp[pos] && regexp[pos] <= '9' )
    {
        int num = 0;

        while ( pos < regexp.size() && '0' <= regexp[pos] && regexp[pos] <= '9' )
        {
            num = 10 * num + regexp[pos] - '0';
            consume();
        }

        return num;
    }
    else
    {
        failWith("Integer expected");
    }
}

Transducer RegexpParser::parseOr()
{
    Transducer t = parseConcat();

    while ( pos < regexp.size() && regexp[pos] == '|' )
    {
        match('|');
        t = Transducer::orTransducer(t, parseConcat());
    }

    return t;
}

IntervalUnion RegexpParser::parseInterval()
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
                failWith("Unmatched range");

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
                case 's': iu.addInterval({' ' ,  ' '}); break;
                case '[': case ']': case '-': case '^': case '\\':
                {
                    iu.addInterval({c2, c2});
                    break;
                }
                default:
                    failWith("Unknown symbol \\" + std::to_string(c2)); 
            }
        }
        else
            iu.addInterval({c, c});
    }        

    match(']');
    return iu;
}

boost::variant<IntervalUnion, char_type> RegexpParser::parseIOPart()
{
    if ( regexp[pos] == '[' )
        return parseInterval();
    else
    {
        auto ch = regexp[pos];
        consume();
        return ch;
    }
}

Transducer RegexpParser::parseBlock()
{
    auto iu = parseInterval();
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
        if ( regexp[pos] != ')' )
        {
            if ( regexp[pos] == '\\' )
            {
                consume();
                if ( pos >= regexp.size() )
                    failWith("Expected symbol after \\");
            }
            t = Transducer::matchChar(regexp[pos]);
            consume();
        }
    }

    if ( regexp[pos] == '*' )
    {
        match('*');
        t = Transducer::klenee(t);
    }
    else if ( regexp[pos] == '{' )
    {
        match('{');
        int from = parseInt();
        match(',');
        int to = parseInt();

        if ( from > to )
            failWith("Incorrect regexp: {m, n} - m > n");

        t = Transducer::timesTransducer(t, from, to);
        match('}');
    }

    return t;
}

Transducer RegexpParser::parseConcat()
{
    Transducer t = parseKlenee();

    while ( pos < regexp.size() && regexp[pos] != '|' && regexp[pos] != ')' )
        t = Transducer::concat(t, parseKlenee());

    return t;
}
