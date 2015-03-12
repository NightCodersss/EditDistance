#include "transducer.hpp"
#include "regexpparser.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include "icu.hpp"

bool operator<(const Transducer::Path& a, const Transducer::Path& b)
{
    return a.cost == b.cost ? a.path < b.path : a.cost < b.cost;
}

string_type screen(string_type s)
{
	string_type res;
	for(auto c: s)
	{
		if(c == '{' || c == '}' || c == '(' || c == ')' || c == '|' || c == '*' || c == '^' || c == '[' || c == ']' || c == '\\' )
			res.push_back('\\');
		res.push_back(c);	
	}
	return res;
}
    
Transducer::Edge::Edge(State* end, IO io, int weight) : end(end), io(io), weight(weight) { }

void Transducer::State::connectTo(Transducer::State* s, IO io, int w)
{
    edges.emplace_back(s, io, w);
}
    
unsigned long long Transducer::State::id() const { return reinterpret_cast<unsigned long long>(this) % 10000; }    
    
Transducer::Transducer()
{
    addState(initial_state = final_state = new State());
    resetMinPaths();
}

// Doesn't accept anything
bool Transducer::isEmpty()
{
	return states.size() == 1 && std::all_of(std::begin(states[0] -> edges), std::end(states[0] -> edges), [&](Transducer::Edge& edge) {
			return edge.io.type == IO::IOType::LetterLetter && edge.io.in == EPS && edge.io.out == EPS && edge.end == states[0];
	});
}

void Transducer::addState(State* newstate)
{
    states.push_back(newstate);
}

void Transducer::addEpsilonTransitions()
{
    for ( auto& state : states )
        state->connectTo(state, IO(EPS, EPS), 0);
}

Transducer Transducer::copy()
{
    Transducer a;
    std::map<State*, State*> newstates;

    for ( auto state : states )
        a.addState(newstates[state] = new State());

    for ( auto state : states )
    {
        for ( auto edge : state -> edges )
            newstates[state] -> connectTo(newstates[edge.end], edge.io, edge.weight);
    }

    a.initial_state = newstates[initial_state];
    a.final_state   = newstates[final_state];

    return a; 
}

Transducer Transducer::composition(Transducer& transducer)
{
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
                if ( e1.io.type == IO::IOType::LetterLetter && e2.io.type == IO::IOType::LetterLetter )
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
                    if ( e1.io.type == IO::IOType::LetterLetter && e2.io.type == IO::IOType::LetterLetter )
                        assert(comp.in == e1.io.in && comp.out == e2.io.out);
                        
                    s->edges.emplace_back(newstates[std::make_pair(e1.end, e2.end)], comp, e1.weight + e2.weight);
                }
            }
        }
    }

    if ( newstates.count(std::make_pair(final_state, transducer.final_state)) == 0 )
        return Transducer();

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
        for ( const auto& edge : state->edges )
            out << (reinterpret_cast<long long>(state) % 10000) << ' '
                << convertFromStringType(edge.io.toString())
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
//        char_type inp, out; 
        UnicodeString inp, out;
        int weight;

        in >> start >> inp >> out >> weight >> end;

        RegexpParser rp1(convertUnicode(inp));
        RegexpParser rp2(convertUnicode(out));
        
        auto io = IO(rp1.parseIOPart(), rp2.parseIOPart());

        new_states[start]->connectTo(new_states[end], io, weight);
    }

    initial_state = new_states[0];
    final_state   = new_states[_final_state];

    for ( auto state : new_states )
        addState(state);

    addEpsilonTransitions();
    resetMinPaths();
}
    
std::vector<Transducer::Edge> Transducer::minWay()
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
        std::cout << convertFromStringType(edge.io.toString()) << ' ' << edge.weight << ' ' 
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
    
Transducer Transducer::matchBlock(IntervalUnion c)
{
    Transducer t;

    t.initial_state = new State();
    t.final_state   = new State();

    t.addState(t.initial_state);
    t.addState(t.final_state);

    t.initial_state -> connectTo(t.final_state, IO(c), 0);
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
    
Transducer Transducer::timesTransducer(Transducer a, int from, int to)
{
    Transducer b;
    
    for ( int i = 0; i < from - 1; ++i )
        b = Transducer::concat(b, a.copy());

    auto end_state = new State();

    if ( from == 0 )
        b.initial_state -> connectTo(end_state, IO(EPS, EPS), 0);

    for ( int i = std::max(from - 1, 0); i < to; ++i )
    {
        auto aa = a.copy();
        aa.final_state -> connectTo(end_state, IO(EPS, EPS), 0);
        b = Transducer::concat(b, aa);
    }

    b.addState(end_state);
    b.final_state = end_state;
    return b;
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
    auto t = RegexpParser().parse(regexp);
    t.addEpsilonTransitions();
    return t;
}

Transducer Transducer::fromAlignmentModel(std::istream& in)
{
	Transducer t;

	int number_of_rules;
	in >> number_of_rules;

	for ( int i = 0; i < number_of_rules; ++i )
	{
		UnicodeString reg, out;
		int weight;

		in >> reg >> out >> weight;

		Transducer regexp_trans = RegexpParser().parse(convertUnicode(reg));
		Transducer string_trans = RegexpParser().parse(screen(convertUnicode(out)));

        for( auto s : regexp_trans.states ) //it empties output of regexp trans
		{
			for( auto& e : s -> edges )
			{
				if ( e.io.type == IO::IOType::UnionUnion )
				{
					e.io.type = IO::IOType::UnionLetter;
					e.io.out = EPS;
				}
				else
				{
					e.io.out = EPS;
				}
			}
		}

		for( auto s : string_trans.states ) //it empties input of string trans
		{
			for( auto& e : s -> edges )
			{
				if( e.io.type == IO::IOType::LetterLetter )
				{
					e.io.in = EPS;
				}
				else
				{
					throw std::logic_error("Second argument of rule must be a string");
				}
			}
		}

		auto logic_trans = concat(regexp_trans, string_trans);

		//importing states
		for(auto s : logic_trans.states) 
			t.addState(s);

		//connecting to t
		t.initial_state -> connectTo(logic_trans.initial_state, IO(EPS, EPS), weight);
		logic_trans.final_state -> connectTo(t.final_state, IO(EPS, EPS), 0);
    }
	return t;
}

void Transducer::resetMinPaths()
{
    paths.clear();
	paths_count.clear();
	paths_count[initial_state] = 1;
    paths.insert(Path{0, initial_state, {}});
}

void Transducer::removeEpsilonEdges()
{
    std::vector<Edge> new_edges;
    for ( auto& state : states )
    {
        new_edges.clear();    
        for ( auto& edge : state -> edges )
        {
            if ( !(edge.io.type == IO::IOType::LetterLetter && edge.io.in == EPS && edge.io.out == EPS && edge.end == state) )
                new_edges.emplace_back(edge.end, edge.io, edge.weight);
        }
        state -> edges = new_edges;
    }
	epsilon_edges_removed = true;	
}

Transducer::Path Transducer::getNextMinPath()
{
    if ( !epsilon_edges_removed )
        removeEpsilonEdges();
    
    printSize(std::cout);

    while ( !paths.empty() && paths_count[final_state] <= MAX_PATHS_SIZE )
    {
        auto p = *std::begin(paths);
        paths.erase(std::begin(paths));
       
        auto u = ((p.path.size() == 0) ? initial_state : (p.path.back() -> end));
        for ( auto& edge : u -> edges )
        {
			if(paths_count[edge.end] > MAX_PATHS_SIZE)
				continue;
            Path pv = p;
            pv.cost += edge.weight;
            pv.path.push_back(&edge);

            paths.insert(pv);
			++paths_count[edge.end];
        }

        if ( u == final_state )
            return p;
    }    
    return Path{-1, nullptr, {}};
}

void Transducer::printSize(std::ostream& out)
{
    out << "Number of vertices: " << states.size() << '\n';
    
    int number_of_edges = 0;
    for ( auto& state : states )
        number_of_edges += state -> edges.size();

    out << "Number of edges: " << number_of_edges << '\n';
}
