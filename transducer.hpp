#ifndef _TRANSDUCER_HPP_
#define _TRANSDUCER_HPP_

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <string>

using char_type = std::string;

const char_type EPS = std::string("ε");

class State;

struct Edge
{
    Edge(State* end, char_type in, char_type out, int weight);

    State* end;
    char_type in;
    char_type out;
    int weight; 
};

class State
{
    friend class Transducer;

public:
    void connectTo(State* s, char_type i, char_type o, int w);

private:
    std::vector<Edge> edges;
};

class Transducer
{
public:

    static Transducer fromRegexp(std::string regexp);

    void addState(State* newstate);
    Transducer composition(Transducer transducer);

    void readFromFile(std::istream& in);    
    void visualize(std::ostream& out);
    
    void setInitialState(State* s);
    void setFinalState(State* s);

    std::vector<Edge> minWay();

    void addEpsilonTransitions();

    static Transducer matchChar(char_type c);
    static Transducer concat(Transducer a, Transducer b);
    static Transducer klenee(Transducer a);
    static Transducer orTransducer(Transducer a, Transducer b);

private:

    State* initial_state;
    State* final_state;
    std::vector<State*> states;
};

#endif
