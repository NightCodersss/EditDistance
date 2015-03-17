#ifndef _TRANSDUCEROPTIMIZER_HPP_
#define _TRANSDUCEROPTIMIZER_HPP_

#include <map>
#include <deque>
#include "io.hpp"

class State;
class Transducer;

class TransducerOptimizer
{
public:
    TransducerOptimizer(Transducer& t);

    void optimize();

private:
    
    void dfs(State* s);
    void precount(State* s);       

    std::vector<Edge> makeOptimizedEdges(std::vector<Edge>& io, State* s);
    void removeUnusedStates();

    Transducer& t;

    std::map<State*, bool> used;
    std::map<State*, int> in_edges_number;
};

#endif
