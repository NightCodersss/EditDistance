#ifndef _INTERVALUNION_HPP_
#define _INTERVALUNION_HPP_

#include <set>

class IntervalUnion
{
public:

    IntervalUnion(bool is_complement);

    void addInterval(std::pair<int, int> interval);

    IntervalUnion intersection(const IntervalUnion& iu);
    IntervalUnion unite(const IntervalUnion& iu);

    bool isIn(int k);

private:

    IntervalUnion internalIntersection(const IntervalUnion& iu);
    IntervalUnion internalUnite(const IntervalUnion& iu);
    IntervalUnion internalDifference(const IntervalUnion& iu);

    bool is_complement;
    std::set< std::pair<int, int> > intervals;
};

#endif
