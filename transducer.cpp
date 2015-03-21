#include "transducer.hpp"
#include "regexpparser.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include "icu.hpp"

ptr<Transducer::State> makeState()
{
    return std::make_unique<Transducer::State>();
}

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

void Transducer::hardDelete()
{
	states.clear();
}
    
unsigned long long Transducer::State::id() const { return reinterpret_cast<unsigned long long>(this) % 10000; }    
    
Transducer::Transducer()
{
    auto st = makeState();
    initial_state = st.get();
    final_state = st.get();
    addState(std::move(st));
    resetMinPaths();
}

Transducer::Transducer(Transducer&& t) : states(std::move(t.states))
{
    initial_state = t.initial_state;
    final_state   = t.final_state;

    resetMinPaths();    
}

Transducer& Transducer::operator=(Transducer&& t)
{
    states        = std::move(t.states);
    initial_state = t.initial_state;
    final_state   = t.final_state;

    return *this;
}

// Doesn't accept anything
bool Transducer::isEmpty()
{
	return states.size() == 1 && std::all_of(std::begin(states[0] -> edges), std::end(states[0] -> edges), [&](Transducer::Edge& edge) {
			return edge.io.type == IO::IOType::LetterLetter && edge.io.in == EPS && edge.io.out == EPS && edge.end == states[0].get();
	});
}

void Transducer::addState(ptr<State> newstate)
{
    states.push_back(std::move(newstate));
}

void Transducer::addEpsilonTransitions()
{
    for ( auto& state : states )
        state -> connectTo(state.get(), IO(EPS, EPS), 0);
}

Transducer Transducer::copy()
{
    Transducer a;
    std::map<State*, State*> newstates;

    for ( const auto& state : states )
    {
        auto new_state = makeState();
        newstates[state.get()] = new_state.get();        
        a.addState(std::move(new_state));
    }

    for ( const auto& state : states )
    {
        for ( const auto& edge : state -> edges )
            newstates[state.get()] -> connectTo(newstates[edge.end], edge.io, edge.weight);
    }

    a.initial_state = newstates[initial_state];
    a.final_state   = newstates[final_state];

    return a; 
}

Transducer Transducer::composition(Transducer transducer)
{
    Transducer product;
    
    std::queue< std::pair<State*, State*> > queue;
    queue.push({initial_state, transducer.initial_state});

    std::map< std::pair<State*, State*>, State* > newstates;
        
    auto s = makeState();
        
    newstates[std::make_pair(initial_state, transducer.initial_state)] = s.get();
    product.addState(std::move(s));

    while ( !queue.empty() )
    {
        State* s1;
        State* s2;

        s1 = queue.front().first;
        s2 = queue.front().second;

        queue.pop();

        State* s = newstates[std::make_pair(s1, s2)];

        for ( const auto& e1 : s1->edges )
        {
            for ( const auto& e2 : s2->edges )
            {
                if ( e1.io.type == IO::IOType::LetterLetter && e2.io.type == IO::IOType::LetterLetter )
                    assert((e1.io.out == e2.io.in) == e1.io.canBeCompositedTo(e2.io));

                if ( e1.io.canBeCompositedTo(e2.io) )
                {
                    if ( !newstates.count({e1.end, e2.end}) )
                    {
                        auto newstate = makeState();
                        
                        newstates[std::make_pair(e1.end, e2.end)] = newstate.get();
                        queue.push(std::make_pair(e1.end, e2.end));
                        
                        product.addState(std::move(newstate));
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
    for ( const auto& state : states )
    {
        for ( const auto& edge : state->edges )
            out << (reinterpret_cast<long long>(state.get()) % 10000) << ' '
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

    states.resize(number_of_states);

    for ( auto& state : states )
        state = makeState();

    for ( int i = 0; i < number_of_transitions; ++i )
    {
        int start, end;
        UnicodeString inp, out;
        int weight;

        in >> start >> inp >> out >> weight >> end;

        RegexpParser rp1(convertUnicode(inp));
        RegexpParser rp2(convertUnicode(out));
        
        auto io = IO(rp1.parseIOPart(), rp2.parseIOPart());

        states[start]->connectTo(states[end].get(), io, weight);
    }

    initial_state = states[0].get();
    final_state   = states[_final_state].get();

    addEpsilonTransitions();
    resetMinPaths();
}
    
std::vector<Transducer::Edge> Transducer::minWay()
{
    const int INF = 1000000000;

    std::set< std::pair<int, State*> > q;
    std::map< State*, int > d;
    std::map< State*, State* > p;
    
    for ( const auto& state : states )
        d[state.get()] = INF;

    d[initial_state] = 0;
    
    for ( const auto& state : states )
    {
        q.insert({d[state.get()], state.get()});
        p[state.get()] = nullptr;
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

    for ( const auto& state : states )
    {
        std::cout << "State:    " << (reinterpret_cast<long long>(state.get()) % 10000) << '\n'
                  << "Distance: " << d[state.get()] << '\n'
                  << "Parent:   " << (reinterpret_cast<long long>(p[state.get()]) % 10000) << '\n';
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

    auto init_state  = makeState();
    auto final_state = makeState();
    
    t.initial_state = init_state.get();
    t.final_state   = final_state.get();

    t.addState(std::move(init_state));
    t.addState(std::move(final_state));

    t.initial_state -> connectTo(t.final_state, IO(c, c), 0);
    return t;
}
    
Transducer Transducer::matchBlock(IntervalUnion c)
{
    Transducer t;

    auto init_state  = makeState();
    auto final_state = makeState();

    t.initial_state = init_state.get();
    t.final_state   = final_state.get();

    t.addState(std::move(init_state));
    t.addState(std::move(final_state));

    t.initial_state -> connectTo(t.final_state, IO(c), 0);
    return t;
}

Transducer Transducer::klenee(Transducer a)
{
    a.final_state -> connectTo(a.initial_state, IO(EPS, EPS), 0);

    auto new_initial = makeState();
    auto new_final   = makeState();

    new_initial -> connectTo(a.initial_state, IO(EPS, EPS), 0);
    new_initial -> connectTo(new_final.get(), IO(EPS, EPS), 0);

    a.final_state -> connectTo(new_final.get(), IO(EPS, EPS), 0);

    a.setInitialState(new_initial.get());
    a.setFinalState(new_final.get());

    a.addState(std::move(new_initial));
    a.addState(std::move(new_final));

    return a;
}

Transducer Transducer::orTransducer(Transducer a, Transducer b)
{
    auto new_initial = makeState();
    auto new_final   = makeState();
    
    Transducer t;

    for ( auto& state : a.states )
        t.addState(std::move(state));

    for ( auto& state : b.states )
        t.addState(std::move(state));

    new_initial -> connectTo(a.initial_state, IO(EPS, EPS), 0);
    new_initial -> connectTo(b.initial_state, IO(EPS, EPS), 0);

    a.final_state -> connectTo(new_final.get(), IO(EPS, EPS), 0);
    b.final_state -> connectTo(new_final.get(), IO(EPS, EPS), 0);

    t.setInitialState(new_initial.get());
    t.setFinalState(new_final.get());

    t.addState(std::move(new_initial));
    t.addState(std::move(new_final));

    return t;
}
    
Transducer Transducer::timesTransducer(Transducer a, int from, int to)
{
    Transducer b;
    
    for ( int i = 0; i < from - 1; ++i )
        b = Transducer::concat(std::move(b), a.copy());

    auto end_state = makeState();

    if ( from == 0 )
        b.initial_state -> connectTo(end_state.get(), IO(EPS, EPS), 0);

    for ( int i = std::max(from - 1, 0); i < to; ++i )
    {
        auto aa = a.copy();
        aa.final_state -> connectTo(end_state.get(), IO(EPS, EPS), 0);
        b = Transducer::concat(std::move(b), std::move(aa));
    }

    b.final_state = end_state.get();
    b.addState(std::move(end_state));
    return b;
}

Transducer Transducer::concat(Transducer a, Transducer b)
{
    for ( auto& state : b.states )
        a.addState(std::move(state));

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

	while(in)
	{
		UnicodeString reg, out;
		int weight;

		in >> reg >> out >> weight;

		Transducer regexp_trans = RegexpParser().parse(convertUnicode(reg));
		Transducer string_trans = RegexpParser().parse(screen(convertUnicode(out)));

        for( const auto& s : regexp_trans.states ) //it empties output of regexp trans
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

		for( const auto& s : string_trans.states ) //it empties input of string trans
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

		auto logic_trans = concat(std::move(regexp_trans), std::move(string_trans));

		//importing states
		for( auto& s : logic_trans.states) 
			t.addState(std::move(s));

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
    for ( const auto& state : states )
    {
        new_edges.clear();    
        for ( auto& edge : state -> edges )
        {
            if ( !(edge.io.type == IO::IOType::LetterLetter && edge.io.in == EPS && edge.io.out == EPS && edge.end == state.get()) )
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
