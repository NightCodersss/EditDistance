#include "transducer.hpp"
#include "regexpparser.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>

IO::IO(const IntervalUnion& u) : type(IOType::UnionUnion), u(u) { }
IO::IO(const IntervalUnion& u, char_type out) : type(IOType::UnionLetter), u(u), out(out) { }
IO::IO(char_type in, char_type out) : type(IOType::LetterLetter), u(false, {}), in(in), out(out) { }
    
std::string IO::toString() const { return "io"; }
bool IO::isEmpty() const { return u.isEmpty() && (type == IOType::UnionUnion || type == IOType::UnionLetter); }

bool IO::canBeCompositedTo(const IO& io) const
{
    return !composition(io).isEmpty(); // not empty intersection
}

IO IO::composition(const IO& io) const
{   
    if ( type == IOType::UnionUnion )
    {
        if ( io.type == IOType::UnionUnion )
            return IO(u.intersection(io.u));
        else if ( io.type == IOType::UnionLetter )
            return IO(u.intersection(io.u), io.out);
        else
        {
            if ( u.isIn(io.in) )
                return IO(io.in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
    }
    else if ( type == IOType::UnionLetter )
    {
        if ( io.type == IOType::UnionUnion )
        {
            if ( io.u.isIn(out) )
                return IO(u, out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else if ( io.type == IOType::UnionLetter )
        {
            if ( io.u.isIn(out) )
                return IO(u, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else //IOType::LetterLetter
        {
            if ( io.in == out )
                return IO(u, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }        
    }
    else //IOType::LetterLetter
    {
        if ( io.type == IOType::UnionUnion )
        {
            if ( io.u.isIn(out) )
                return IO(in, out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else if ( io.type == IOType::UnionLetter )
        {
            if ( io.u.isIn(out) )
                return IO(in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
        else //IOType::LetterLetter
        {
            if ( io.in == out )
                return IO(in, io.out);
            else
                return IO(IntervalUnion(false, { }));
        }
    }
}

Edge::Edge(State* end, IO io, int weight) : end(end), io(io), weight(weight) { }

void State::connectTo(State* s, IO io, int w)
{
    edges.emplace_back(s, io, w);
}
    
unsigned long long State::id() const { return reinterpret_cast<unsigned long long>(this) % 10000; }    

void Transducer::addState(State* newstate)
{
    states.push_back(newstate);
}

void Transducer::addEpsilonTransitions()
{
    for ( auto& state : states )
        state->connectTo(state, IO(EPS, EPS), 0);
}
    
Transducer Transducer::composition(Transducer transducer) const
{
    transducer.addEpsilonTransitions();
        
    Transducer product;
    
    std::queue< std::pair<State*, State*> > queue;
    queue.push({initial_state, transducer.initial_state});

    std::map< std::pair<State*, State*>, State* > newstates;
        
    State *s = new State();
    product.addState(s);
        
    newstates[std::make_pair(initial_state, transducer.initial_state)] = s;

    while ( !queue.empty() )
    {
        State* s1;
        State* s2;

        s1 = queue.front().first;
        s2 = queue.front().second;

        queue.pop();

        State* s = newstates[std::make_pair(s1, s2)];

        auto assert = [](bool x) { if ( !x ) throw "pizdoh"; };

        for ( auto e1 : s1->edges )
        {
            for ( auto e2 : s2->edges )
            {
                assert((e1.io.out == e2.io.in) == e1.io.canBeCompositedTo(e2.io));

                if ( e1.io.canBeCompositedTo(e2.io) )
                {
                    if ( !newstates.count({e1.end, e2.end}) )
                    {
                        State* newstate = new State();
                        product.addState(newstate);
                        
                        newstates[std::make_pair(e1.end, e2.end)] = newstate;
                        queue.push(std::make_pair(e1.end, e2.end));
                    }

                    auto comp = e1.io.composition(e2.io);
                    assert(comp.in == e1.io.in && comp.out == e2.io.out);
                        
                    s->edges.emplace_back(newstates[std::make_pair(e1.end, e2.end)], comp, e1.weight + e2.weight);
                }
            }
        }
    }

    std::cout << "States:\n";
    for ( auto state : newstates )
    {
        std::cout << "Key: (" << state.first.first -> id() << ", " << state.first.second -> id() << ")\n";
        std::cout << "State: " << state.second -> id() << '\n';
    }

    std::cout << "Final states:\n";
    std::cout << "(" << final_state -> id() << ", " << transducer.final_state -> id() << ")\n";

    product.initial_state = newstates.at(std::make_pair(initial_state, transducer.initial_state));
    product.final_state = newstates.at(std::make_pair(final_state, transducer.final_state));

    return product;
}
    
void Transducer::visualize(std::ostream& out)
{
    out << "Transducer:\n";
    out << "Initial state: " << (reinterpret_cast<long long>(initial_state) % 10000) << '\n' 
        << "Final state: " << (reinterpret_cast<long long>(final_state) % 10000) << '\n';
    for ( auto state : states )
    {
        for ( auto edge : state->edges )
            out << (reinterpret_cast<long long>(state) % 10000) << ' '
                << edge.io.toString()
                << '/' 
                << edge.weight 
                << ' '
                << (reinterpret_cast<long long>(edge.end) % 10000) 
                << '\n';
    }
}
    
void Transducer::setInitialState(State* s)
{
    initial_state = s;
}
    
void Transducer::setFinalState(State* s)
{
    final_state = s;
}

void Transducer::readFromFile(std::istream& in)
{
    states.clear();

    int number_of_states, number_of_transitions, _final_state;
    in >> number_of_states >> number_of_transitions >> _final_state;

    std::vector<State*> new_states(number_of_states);
    for ( auto& state : new_states )
        state = new State();

    for ( int i = 0; i < number_of_transitions; ++i )
    {
        int start, end;
        char_type inp, out; 
        int weight;

        in >> start >> inp >> out >> weight >> end;
        new_states[start]->connectTo(new_states[end], IO(inp, out), weight);
    }

    initial_state = new_states[0];
    final_state   = new_states[_final_state];

    for ( auto state : new_states )
        addState(state);
}
    
std::vector<Edge> Transducer::minWay()
{
    const int INF = 1000000000;

    std::set< std::pair<int, State*> > q;
    std::map< State*, int > d;
    std::map< State*, State* > p;
    
    for ( auto state : states )
        d[state] = INF;

    d[initial_state] = 0;
    
    for ( auto state : states )
    {
        q.insert({d[state], state});
        p[state] = nullptr;
    }

    while ( !q.empty() )
    {
        State* s = std::begin(q) -> second;
        q.erase(std::begin(q));

        for ( auto edge : s -> edges )
        {
            if ( d[edge.end] > d[s] + edge.weight )
            {
                q.erase({d[edge.end], edge.end});
                d[edge.end] = d[s] + edge.weight;
                q.insert({d[edge.end], edge.end});

                p[edge.end] = s;
            }
        }
    }

    for ( auto state : states )
    {
        std::cout << "State:    " << (reinterpret_cast<long long>(state) % 10000) << '\n'
                  << "Distance: " << d[state] << '\n'
                  << "Parent:   " << (reinterpret_cast<long long>(p[state]) % 10000) << '\n';
        std::cout << '\n';
    }

    std::vector<Edge> path;
    for ( State* s = final_state; p[s] != NULL; s = p[s] )
    {
        State* q = p[s];

        for ( auto edge : q -> edges )
        {
            if ( edge.end == s )
            {
                path.push_back(edge);
                break;
            }
        }
    }
    std::reverse(std::begin(path), std::end(path));

    std::cout << "...............................\n\n";
    std::cout << "Path: \n";

    std::cout << "Initial state: " << (reinterpret_cast<long long>(initial_state) % 10000) << '\n';
    for ( auto edge : path )
    {
        std::cout << edge.io.toString() << ' ' << edge.weight << ' ' 
        << (reinterpret_cast<long long>(edge.end) % 10000) << '\n';
    }
    std::cout << '\n';

    return path;
}

Transducer Transducer::matchChar(char_type c)
{
    Transducer t;

    t.initial_state = new State();
    t.final_state   = new State();

    t.addState(t.initial_state);
    t.addState(t.final_state);

    t.initial_state -> connectTo(t.final_state, IO(c, c), 0);
    return t;
}

Transducer Transducer::klenee(Transducer a)
{
    a.final_state -> connectTo(a.initial_state, IO(EPS, EPS), 0);

    State* new_initial = new State();
    State* new_final   = new State();

    new_initial -> connectTo(a.initial_state, IO(EPS, EPS), 0);
    new_initial -> connectTo(new_final, IO(EPS, EPS), 0);

    a.final_state -> connectTo(new_final, IO(EPS, EPS), 0);

    a.addState(new_initial);
    a.addState(new_final);

    a.setInitialState(new_initial);
    a.setFinalState(new_final);

    return a;
}

Transducer Transducer::orTransducer(Transducer a, Transducer b)
{
    State* new_initial = new State();
    State* new_final   = new State();

    Transducer t;

    for ( auto state : a.states )
        t.addState(state);

    for ( auto state : b.states )
        t.addState(state);

    t.addState(new_initial);
    t.addState(new_final);

    new_initial -> connectTo(a.initial_state, IO(EPS, EPS), 0);
    new_initial -> connectTo(b.initial_state, IO(EPS, EPS), 0);

    a.final_state -> connectTo(new_final, IO(EPS, EPS), 0);
    b.final_state -> connectTo(new_final, IO(EPS, EPS), 0);

    t.setInitialState(new_initial);
    t.setFinalState(new_final);

    return t;
}

Transducer Transducer::concat(Transducer a, Transducer b)
{
    for ( auto state : b.states )
        a.addState(state);

    a.final_state -> connectTo(b.initial_state, IO(EPS, EPS), 0);
    a.setFinalState(b.final_state);

    return a;
}

Transducer Transducer::fromRegexp(string_type regexp)
{
    return RegexpParser().parse(regexp);
}
