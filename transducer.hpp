#ifndef _TRANSDUCER_HPP_
#define _TRANSDUCER_HPP_

#include <iostream>
#include <vector>
#include <queue>
#include <map>

class State;

struct Edge
{
    Edge(State* end, char in, char out, int weight);

    State* end;
    char in;
    char out;
    int weight; 
};

class State
{
    friend class Transducer;

public:
    void connectTo(State* s, char i, char o, int w);

private:
    std::vector<Edge> edges;
};

class Transducer
{
public:

    void addState(State* newstate);
    Transducer composition(Transducer transducer);

    void readFromFile(std::istream& in);    
    void visualize(std::ostream& out);
    
    void setInitialState(State* s);

private:

    State* initial_state;
    std::vector<State*> states;
};

#endif
