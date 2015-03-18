#include "transduceroptimizer.hpp"
#include "transducer.hpp"
#include <functional>

TransducerOptimizer::TransducerOptimizer(Transducer& t) : t(t) { }

void TransducerOptimizer::optimize()
{
    t.removeEpsilonEdges();
    precount();

    dfs(t.initial_state);
    removeUnusedStates();

    t.addEpsilonTransitions();
}

void TransducerOptimizer::precount()
{
    std::map<Transducer::State*, bool> used;

    std::function<void(Transducer::State*)> dfs = [&](Transducer::State* s)
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

    dfs(t.initial_state);
}

void TransducerOptimizer::dfs(Transducer::State* s)
{
    static std::deque<Transducer::Edge> path_edges;

    if ( used[s] )
        return;

    used[s] = true;

    if ( in_edges_number[s] == 1 && s -> edges.size() == 1 )
    {
        const auto& edge = s -> edges[0];

        dfs(edge.end);
        path_edges.push_front(edge);
    }
    else
    {
        std::vector<Transducer::Edge> new_edges;

        for ( const auto& edge : s -> edges )
        {
            if ( used[edge.end] )
            {
                new_edges.push_back(edge);
                continue;
            }

            dfs(edge.end);

            auto new_edge = edge;

            if ( !path_edges.empty() )
            {
                path_edges.push_front(edge);
                auto edges = makeOptimizedEdges(path_edges, s);
                new_edge = edges[0];

                for ( const auto& edge : edges )
                    t.addState(edge.end);

                path_edges.clear();
            }

            new_edges.push_back(new_edge);
        }

        s -> edges = new_edges;
    }
}

std::vector<Transducer::Edge> TransducerOptimizer::makeOptimizedEdges(const std::deque<Transducer::Edge>& edges, Transducer::State* first_state)
{
    auto s = edges.back().end;

    std::vector<char_type> in, out;
    std::vector<Transducer::Edge> result;

    int weight = 0;
    for ( const auto& edge : edges )
    {
        weight += edge.weight;

        if ( edge.io.type == IO::IOType::LetterLetter )
        {
            if ( edge.io.in != EPS )
                in.push_back(edge.io.in);

            if ( edge.io.out != EPS )
                out.push_back(edge.io.out);
        }
        else
        {
            while ( in.size() < out.size() )
                in.push_back(EPS);

            while ( out.size() < in.size() )
                out.push_back(EPS);

            for ( size_t i = 0; i < in.size(); ++i )
                result.push_back(Transducer::Edge(nullptr, IO(in[i], out[i]), 0));

            in.clear();
            out.clear();

            result.push_back(Transducer::Edge(nullptr, edge.io, 0));
        }
    }

    while ( in.size() < out.size() )
        in.push_back(EPS);

    while ( out.size() < in.size() )
        out.push_back(EPS);

    for ( size_t i = 0; i < in.size(); ++i )
        result.push_back(Transducer::Edge(nullptr, IO(in[i], out[i]), 0));

    if ( weight != 0 || (in_edges_number[s] > 1 && first_state -> edges.size() > 1) )
    {
        if ( result.empty() )
            result.push_back(Transducer::Edge(nullptr, IO(EPS, EPS), 0));

        result[0].weight = weight;
    }

    result.back().end = s;

    for ( int i = static_cast<int>(result.size()) - 2; i >= 0; --i )
    {
        auto& edge = result[i];

        edge.end = new Transducer::State();
        edge.end -> edges = {result[i + 1]};
    }

    return result;
}

void TransducerOptimizer::removeUnusedStates()
{   
    std::map<Transducer::State*, bool> used, can_go_to_final;

    can_go_to_final[t.final_state] = true;

    std::function<void(Transducer::State*)> dfs = [&](Transducer::State* s)
    {
        if ( used[s] )
            return;

        used[s] = true;

        for ( const auto& edge : s -> edges )
        {
            dfs(edge.end);
            can_go_to_final[s] = can_go_to_final[s] || can_go_to_final[edge.end];
        }
    };

    dfs(t.initial_state);

    std::vector<Transducer::State*> new_states;
    std::set<Transducer::State*> unused_states;

    for ( auto state : t.states )
    {
        if ( used[state] && can_go_to_final[state] )
            new_states.push_back(state);
        else
            unused_states.insert(state);
    }

    t.states = new_states;

    for ( auto state : t.states )
    {
        std::vector<Transducer::Edge> new_edges;
        for ( const auto& edge : state -> edges )
        {
            if ( !unused_states.count(edge.end) )
                new_edges.push_back(edge);
        }
        state -> edges = new_edges;
    }
}
