#ifndef _INTERVALUNION_HPP_
#define _INTERVALUNION_HPP_

#include <set>
#include <string>

class IntervalUnion
{
public:

    IntervalUnion(bool is_complement, std::set< std::pair<int, int> > intervals);

    void addInterval(std::pair<int, int> interval);

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
    std::set< std::pair<int, int> > intervals;
};

#endif
