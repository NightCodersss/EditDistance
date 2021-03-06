#ifndef _IO_HPP_
#define _IO_HPP_

#include <boost/variant.hpp>
#include "intervalunion.hpp"

using IOPart = boost::variant<IntervalUnion, char_type>;

struct IO
{
    enum class IOType { UnionUnion, UnionLetter, LetterLetter };

    IO(IOPart inp, IOPart out);
    IO(const IntervalUnion& u);
    IO(const IntervalUnion& u, char_type out);
    IO(char_type in, char_type out);

    bool canBeCompositedTo(const IO& io) const;
    IO composition(const IO& io) const;

    string_type toString() const;
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
