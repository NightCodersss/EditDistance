#include "transduceroptimizer.hpp"
#include "transducer.hpp"

TransducerOptimizer::TransducerOptimizer(Transducer& t) : t(t) { }

void TransducerOptimizer::optimize()
{
    t -> removeEpsilonEdges();
    precount();

    dfs(t -> initial_state);
    removeUnusedStates();    

    t -> addEpsilonTransitions();
}

void TransducerOptimizer::precount()
{
    std::map<State*, bool> used;

    auto dfs = [&](auto&& s)
    {
        if ( used[s] )
            return;

        used[s] = true;

        for ( const auto& edge : s -> edges )
        {
            ++in_edges_number[edge.end];
            if ( !used[edge.end] )
                dfs(edge.end);            
        }
    };

    dfs(t -> initial_state);
}
    
void TransducerOptimizer::dfs(State* s, std::deque<Edge>& io, State*& end_state)
{
    if ( used[s] )
        return;
    
    used[s] = true;

    if ( in_edges_number[s] == 1 && s -> edges.size() == 1 )
    {
        const auto& edge = s -> edges[0];
            
        if ( used[edge.end] )
            new_end_state = edge.end;

        dfs(edge.end, io, end_state);
        io.push_front(edge);
    }
    else
    {
        end_state = s;        

        State* new_end_state = nullptr;

        std::vector<Edge> new_edges;

        for ( const auto& edge : s -> edges )
        {
            if ( used[edge.end] )
            {
                new_edges.push_back(edge);
                continue;
            }

            dfs(edge.end, io, new_end_state);
            if ( !io.empty() )
            {
                io.push_back(edge);
                auto edges = makeOptimizedEdges(io, new_end_state);
                new_edges.push_back(edges[0]);

                for ( const auto& state : states )
                    t -> addState(state);

                io = { };
            }
            else
                new_edges.push_back(edge);
        }

        s -> edges = new_edges;
    }
}
    
std::vector<Edge> TransducerOptimizer::makeOptimizedEdges(std::deque<Edge>& edges, State* s)
{
    std::vector<char_type> in, out;
    std::vector<Edge> result;

    int weight = 0;
    for ( const auto& edge : edges )
        weight += edge.weight; 

    int pos = 0;
    while ( pos < io.size() )
    {
        const auto& edge = edges[pos];

        if ( edge.io.type == IO::IOType::UnionUnion || edge.io.type == IO::IOType::UnionLetter )
        {
            while ( in.size() < out.size() )
                in.push_back(EPS);

            while ( out.size() < in.size() )
                out.push_back(EPS);

            for ( int i = 0; i < in.size(); ++i )
                result.push_back(Edge(nullptr, IO(in[i], out[i]), 0));

            in.clear();
            out.clear();

            result.push_back(Edge(nullptr, edges[pos].io, 0));            
        }
        else
        {
            if ( edge.io.in != EPS )
                in.push_back(edge.io.in);

            if ( edge.io.out != EPS )
                out.push_back(edge.io.out);
        }

        ++pos;
    }
            
    while ( in.size() < out.size() )
        in.push_back(EPS);

    while ( out.size() < in.size() )
        out.push_back(EPS);

    for ( int i = 0; i < in.size(); ++i )
        result.push_back(Edge(nullptr, IO(in[i], out[i]), 0));
    
    if ( weight != 0 )
    {
        if ( result.empty() )
            result.push_back(Edge(nullptr, IO(EPS, EPS), 0));

        result[0].weight = weight;
    }
    
    for ( int i = 0; i < static_cast<int>(result.size()) - 1; ++i )
    {
        edge.end = new State();
        edge.end -> edges = {result[i + 1]};
    }
    result.back().end = s;

    return result;    
}

void TransducerOptimizer::removeUnusedStates()
{
    std::map<State*, bool> used, can_go_to_final;

    can_go_to_final[t -> final_state] = true;

    auto dfs = [&](auto&& s)
    {
        if ( used[s] )
            return;

        used[s] = true;

        for ( const auto& edge : s -> edges )
        {
            if ( !used[edge.end] )
                dfs(edge.end);            
            can_go_to_final[s] = can_go_to_final[s] || can_go_to_final[edge.end];
        }
    };

    dfs(t -> initial_state);

    std::vector<State*> new_states;
    std::set<State*> unused_states;

    for ( auto state : t -> states )
    {
        if ( used[state] && can_go_to_final[state] )
            new_states -> push_back(state);
        else
            unused_states.insert(state);
    }

    t -> states = new_states;

    for ( auto state : t -> states )
    {
        std::vector<Edge> new_edges;
        for ( const auto& edge : state -> edges )
        {
            if ( !unused_states.count(edge.end) )
                new_edges.push_back(edge);
        }
        state -> edges = new_edges;
    }
}
