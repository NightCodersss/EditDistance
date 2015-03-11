#include "io.hpp"
#include <cassert>

IO::IO(IOPart i, IOPart o) : u(false, {})
{
    if ( i.which() == 0 )
    {
        if ( o.which() == 0 )
        {
            type = IOType::UnionUnion;
            u    = boost::get<IntervalUnion>(i);

            assert(boost::get<IntervalUnion>(i) == boost::get<IntervalUnion>(o));
        }
        else
        {
            type = IOType::UnionLetter;
            u    = boost::get<IntervalUnion>(i);
            out  = boost::get<char_type>(o);            
        }
    }
    else
    {
        type = IOType::LetterLetter;
        in   = boost::get<char_type>(i);
        out  = boost::get<char_type>(o);
    }
}

IO::IO(const IntervalUnion& u) : type(IOType::UnionUnion), u(u) { }
IO::IO(const IntervalUnion& u, char_type out) : type(IOType::UnionLetter), u(u), out(out) { }
IO::IO(char_type in, char_type out) : type(IOType::LetterLetter), u(false, {}), in(in), out(out) { }
    
string_type IO::toString() const 
{
    switch ( type )
    {
    case IOType::UnionUnion:
    case IOType::UnionLetter:
    {
        string_type result;
        result.emplace_back('[');
        if ( u.is_complement )
            result.emplace_back('^');

        for ( const auto& p : u.intervals )
        {
            result.emplace_back(p.first);
            if ( p.first != p.second )
            {
                result.emplace_back('-');
                result.emplace_back(p.second);
            }
        }

        result.emplace_back(']');
        result.emplace_back(' ');
        
        if ( type == IOType::UnionUnion ) 
            result.insert(std::end(result), std::begin(result), std::prev(std::end(result)));
        else
            result.emplace_back(out);
        return result;
    }
    case IOType::LetterLetter:
    {
        string_type result;
        result.emplace_back(in != EPS ? in : EPS_char);
        result.emplace_back(' ');
        result.emplace_back(out != EPS ? out : EPS_char);
        return result;
    }
    }

    return string_type();
}

bool IO::isEmpty() const { return u.isEmpty() && (type == IOType::UnionUnion || type == IOType::UnionLetter); }

bool IO::canBeCompositedTo(const IO& io) const
{
    return !composition(io).isEmpty(); // not empty intersection
}

IO IO::composition(const IO& io) const
{   
    if ( type == IOType::UnionUnion )
    {
        if ( io.type == IOType::UnionUnion )
            return IO(u.intersection(io.u));
        else if ( io.type == IOType::UnionLetter )
            return IO(u.intersection(io.u), io.out);
        else
        {
            if ( u.isIn(io.in) )
                return IO(io.in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
    }
    else if ( type == IOType::UnionLetter )
    {
        if ( io.type == IOType::UnionUnion )
        {
            if ( io.u.isIn(out) )
                return IO(u, out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else if ( io.type == IOType::UnionLetter )
        {
            if ( io.u.isIn(out) )
                return IO(u, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else //IOType::LetterLetter
        {
            if ( io.in == out )
                return IO(u, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }        
    }
    else //IOType::LetterLetter
    {
        if ( io.type == IOType::UnionUnion )
        {
            if ( io.u.isIn(out) )
                return IO(in, out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else if ( io.type == IOType::UnionLetter )
        {
            if ( io.u.isIn(out) )
                return IO(in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else //IOType::LetterLetter
        {
            if ( io.in == out )
                return IO(in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
    }
}

