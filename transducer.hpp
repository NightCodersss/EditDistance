#ifndef _TRANSDUCER_HPP_
#define _TRANSDUCER_HPP_

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <string>
#include "chartype.hpp"
#include "intervalunion.hpp"
#include "io.hpp"

class Transducer
{
public:

    class State;

    struct Edge
    {
        Edge(State* end, IO io, int weight);

        State* end;
        IO io;
        int weight; 
    };

    class State
    {
        friend class Transducer;

    public:
        void connectTo(State* s, IO io, int w);

        unsigned long long id() const;

    private:
        std::vector<Edge> edges;
    };

    struct Path
    {
        int cost;
        State* initial;
        std::vector<Edge*> path;
    };

    Transducer();

    //No destructor because states are copied between different transducers
    //Need to add smart pointers later

    static Transducer fromRegexp(string_type regexp);

    void addState(State* newstate);
    Transducer composition(Transducer& transducer);

    void readFromFile(std::istream& in);    
    void visualize(std::ostream& out);

    std::vector<Edge> minWay();
    
    void setInitialState(State* s);
    void setFinalState(State* s);

    void addEpsilonTransitions();

    static Transducer matchChar(char_type c);
    static Transducer matchBlock(IntervalUnion c);
    static Transducer concat(Transducer a, Transducer b);
    static Transducer klenee(Transducer a);
    static Transducer orTransducer(Transducer a, Transducer b);

    void resetMinPaths();
    Path getNextMinPath();

private:

    State* initial_state;
    State* final_state;
    std::vector<State*> states;
    
    std::set<Path> paths;
};

bool operator<(const Transducer::Path& a, const Transducer::Path& b);

#endif
