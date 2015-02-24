#include "intervalunion.hpp"
#include <vector>
#include <algorithm>
#include <tuple>

IntervalUnion::IntervalUnion(bool is_complement, std::set< IntervalType > intervals) : is_complement(is_complement), intervals(intervals)
{

}

IntervalUnion IntervalUnion::changeComplement()
{
    is_complement ^= 1;
    return *this;
}

void IntervalUnion::addInterval(IntervalType interval)
{
    intervals.insert(interval);
}

bool IntervalUnion::isEmpty() const { return intervals.empty(); }

IntervalUnion IntervalUnion::intersection(const IntervalUnion& iu) const
{
    if ( !is_complement && !iu.is_complement )
        return internalIntersection(iu);
    else if ( is_complement && iu.is_complement )
        return internalUnite(iu).changeComplement();
    else if ( is_complement && !iu.is_complement )
        return iu.internalDifference(*this);
    else
        return internalDifference(iu);
}

IntervalUnion IntervalUnion::unite(const IntervalUnion& iu) const
{
    if ( !is_complement && !iu.is_complement )
        return internalUnite(iu);
    else if ( is_complement && iu.is_complement )
        return internalIntersection(iu).changeComplement();
    else if ( is_complement && !iu.is_complement )
        return internalDifference(iu).changeComplement();
    else
        return iu.internalDifference(*this).changeComplement();
}

IntervalUnion IntervalUnion::internalIntersection(const IntervalUnion& iu) const
{
    std::vector< IntervalType > v;

    std::set< IntervalType > ans;

    for ( auto interval : intervals )
    {
        v.push_back({interval.first , 0});
        v.push_back({interval.second, 1});
    }

    for ( auto interval : iu.intervals )
    {
        v.push_back({interval.first , 0});
        v.push_back({interval.second, 1});
    }

    std::sort(std::begin(v), std::end(v));

    int balance = 0;

    int lastX = v.front().first;
    for ( auto event : v )
    {
        if ( event.second == 0 )
        {
            lastX = event.first;
            ++balance;
        }
        else
        {
            --balance;
            if ( balance )
                ans.insert({lastX, event.first});
        }
    }

    return IntervalUnion(false, ans);
}

IntervalUnion IntervalUnion::internalUnite(const IntervalUnion& iu) const
{
    std::vector< IntervalType > v;

    std::set< IntervalType > ans;

    for ( auto interval : intervals )
    {
        v.push_back({interval.first , 0});
        v.push_back({interval.second, 1});
    }

    for ( auto interval : iu.intervals )
    {
        v.push_back({interval.first , 0});
        v.push_back({interval.second, 1});
    }

    std::sort(std::begin(v), std::end(v));

    int firstX = v.front().first;
    
    int open = 0;
    int closed = 0;

    for ( auto event : v )
    {
        if ( event.second == 0 )
        {
            if ( open == 0 )
                firstX = event.first;
            ++open;
        }
        else
        {
            ++closed;
            if ( open == closed )
            {
                open = 0;
                closed = 0;
                ans.insert({firstX, event.first});
            }
        }
    }

    return IntervalUnion(false, ans);
}

bool IntervalUnion::isIn(int x) const
{
    bool is_there = false;
    for ( auto interval : intervals )
    {
        if ( interval.first <= x && x <= interval.second )
        {
            is_there = true;
            break;
        }
    }

    return is_there ^ is_complement;
}

IntervalUnion IntervalUnion::internalDifference(const IntervalUnion& iu) const
{
    std::vector< std::tuple<int, int, int> > v;

    std::set< IntervalType > ans;

    for ( auto interval : intervals )
    {
        v.push_back(std::make_tuple(interval.first , 0, 0));
        v.push_back(std::make_tuple(interval.second, 1, 0));
    }

    for ( auto interval : iu.intervals )
    {
        v.push_back(std::make_tuple(interval.first , 0, 1));
        v.push_back(std::make_tuple(interval.second, 1, 1));
    }

    std::sort(std::begin(v), std::end(v));

    IntervalType last_start;    

    int first = 0;
    int second = 0;

    for ( auto event : v )
    {
        if ( std::get<2>(event) == 0 ) // first union
        {
            if ( std::get<1>(event) == 0 ) // open
            {
                ++first;
                if ( second == 0 )
                    last_start = {std::get<0>(event), std::get<1>(event)};
            }
            else // closed
            {
                --first;
                if ( second == 0 )
                    ans.insert({last_start.first, std::get<0>(event)});
            }
        }
        else // second union
        {
            if ( std::get<1>(event) == 0 ) // open
            {
                ++second;
                if ( first > 0 )
                    ans.insert({last_start.first, std::get<0>(event)});
            }
            else // closed
            {
                --second;
                if ( first > 0 )
                    last_start = {std::get<0>(event), std::get<1>(event)};
            }
        }
    }

    return IntervalUnion(false, ans);
}
    
std::string IntervalUnion::toString() const
{
    std::string s = "";
    if ( is_complement )
        s += "Complement\n";

    for ( auto i : intervals )
        s += "[" + std::to_string(i.first) + ", " + std::to_string(i.second) + "] ";

    s += '\n';
    return s;
}
    
bool IntervalUnion::operator==(const IntervalUnion& iu)
{
    return is_complement == iu.is_complement && intervals == iu.intervals;
}
