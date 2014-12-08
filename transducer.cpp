#include "transducer.hpp"
#include <iostream>

Edge::Edge(State* end, char_type in, char_type out, int weight) : end(end), in(in), out(out), weight(weight) { }

void State::connectTo(State* s, char_type i, char_type o, int w)
{
    edges.emplace_back(s, i, o, w);
}

void Transducer::addState(State* newstate)
{
    states.push_back(newstate);
}
    
Transducer Transducer::composition(Transducer transducer)
{
    for ( auto& state : transducer.states )
        state->connectTo(state, EPS, EPS, 0);

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

        for ( auto e1 : s1->edges )
        {
            for ( auto e2 : s2->edges )
            {
                if ( e1.out == e2.in )
                {
                    if ( !newstates.count({e1.end, e2.end}))
                    {
                        State* newstate = new State();
                        product.addState(newstate);
                        
                        newstates[std::make_pair(e1.end, e2.end)] = newstate;
                        queue.push(std::make_pair(e1.end, e2.end));
                    }
                        
                    s->edges.emplace_back(newstates[std::make_pair(e1.end, e2.end)], e1.in, e2.out, e1.weight + e2.weight);
                }
            }
        }
    }

    product.initial_state = newstates[std::make_pair(initial_state, transducer.initial_state)];

    return product;
}
    
void Transducer::visualize(std::ostream& out)
{
    out << "Transducer:\n";
    out << "Initial state: " << (reinterpret_cast<long long>(initial_state) % 10000) << '\n';
    for ( auto state : states )
    {
        for ( auto edge : state->edges )
            out << (reinterpret_cast<long long>(state) % 10000)
                << edge.in 
                << ':' 
                << edge.out 
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

void Transducer::readFromFile(std::istream& in)
{
    states.clear();

    int number_of_states, number_of_transitions;
    in >> number_of_states >> number_of_transitions;

    std::vector<State*> new_states(number_of_states);
    for ( auto& state : new_states )
        state = new State();

    for ( int i = 0; i < number_of_transitions; ++i )
    {
        int start, end;
        char_type inp, out; 
        int weight;

        in >> start >> inp >> out >> weight >> end;
        new_states[start]->connectTo(new_states[end], inp, out, weight);
    }

    initial_state = new_states[0];
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
                q.erase({d[s], s});
                d[edge.end] = d[s] + edge.weight;
                q.insert({d[s], s});

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

    return { };
}
