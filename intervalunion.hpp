#ifndef _INTERVALUNION_HPP_
#define _INTERVALUNION_HPP_

#include <set>
#include <string>
#include "chartype.hpp"

using IntervalType = std::pair<char_type, char_type>;

class IntervalUnion
{
public:

    IntervalUnion(bool is_complement, std::set<IntervalType> intervals);

    void addInterval(IntervalType interval);

    IntervalUnion changeComplement();

    IntervalUnion intersection(const IntervalUnion& iu) const;
    IntervalUnion unite(const IntervalUnion& iu) const;

    bool isIn(int k) const;
    bool isEmpty() const;

    std::string toString() const;

private:

    IntervalUnion internalIntersection(const IntervalUnion& iu) const;
    IntervalUnion internalUnite(const IntervalUnion& iu) const;
    IntervalUnion internalDifference(const IntervalUnion& iu) const;

    bool is_complement;
    std::set<IntervalType> intervals;
};

#endif
