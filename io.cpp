#include "io.hpp"

IO::IO(const IntervalUnion& u) : type(IOType::UnionUnion), u(u) { }
IO::IO(const IntervalUnion& u, char_type out) : type(IOType::UnionLetter), u(u), out(out) { }
IO::IO(char_type in, char_type out) : type(IOType::LetterLetter), u(false, {}), in(in), out(out) { }
    
std::string IO::toString() const { return "io"; }
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

