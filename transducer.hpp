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

class State;

struct IO
{
    enum class IOType { UnionUnion, UnionLetter, LetterLetter };

    IO(const IntervalUnion& u);
    IO(const IntervalUnion& u, char_type out);
    IO(char_type in, char_type out);

    bool canBeCompositedTo(const IO& io) const;
    IO composition(const IO& io) const;

    std::string toString() const;
    bool isEmpty() const;

    IOType type;

// if output is IU (interval union) then input == output == u
// if output is letter, input == u && output == out
// if input is letter then input == in && output == out

    IntervalUnion u;

    char_type in;
    char_type out;
};

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

class Transducer
{
public:

    Transducer();

    //No destructor because states are copied between different transducers
    //Need to add smart pointers later

    static Transducer fromRegexp(string_type regexp);

    void addState(State* newstate);
    Transducer composition(Transducer& transducer);

    void readFromFile(std::istream& in);    
    void visualize(std::ostream& out);
    
    void setInitialState(State* s);
    void setFinalState(State* s);

    std::vector<Edge> minWay();

    void addEpsilonTransitions();

    static Transducer matchChar(char_type c);
    static Transducer matchBlock(IntervalUnion c);
    static Transducer concat(Transducer a, Transducer b);
    static Transducer klenee(Transducer a);
    static Transducer orTransducer(Transducer a, Transducer b);

private:

    State* initial_state;
    State* final_state;
    std::vector<State*> states;
};

#endif
