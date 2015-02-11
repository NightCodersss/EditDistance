#ifndef _IO_HPP_
#define _IO_HPP_

#include "intervalunion.hpp"

struct IO
{
    enum class IOType { UnionUnion, UnionLetter, LetterLetter };

    IO(const IntervalUnion& u);
    IO(const IntervalUnion& u, char_type out);
    IO(char_type in, char_type out);

    bool canBeCompositedTo(const IO& io) const;
    IO composition(const IO& io) const;

    std::string toString() const;
    bool isEmpty() const;

    IOType type;

// if output is IU (interval union) then input == output == u
// if output is letter, input == u && output == out
// if input is letter then input == in && output == out

    IntervalUnion u;

    char_type in;
    char_type out;
};

#endif
