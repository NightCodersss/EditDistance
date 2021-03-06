#ifndef _TRANSDUCEROPTIMIZER_HPP_
#define _TRANSDUCEROPTIMIZER_HPP_

#include <map>
#include <deque>
#include "io.hpp"
#include "transducer.hpp"

class TransducerOptimizer
{
public:
    TransducerOptimizer(Transducer& t);

    void optimize();

private:

    void dfs(Transducer::State* s);
    void precount();

    std::vector<Transducer::Edge> makeOptimizedEdges(const std::deque<Transducer::Edge>& io, Transducer::State* first_state);
    void removeUnusedStates();

    Transducer& t;

    std::map<Transducer::State*, bool> used;
    std::map<Transducer::State*, int> in_edges_number;
};

#endif
