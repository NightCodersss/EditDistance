#include <iostream>
#include "transducer.hpp"

int main()
{
    Transducer t1;

    {
        State* s0 = new State();
        State* s1 = new State();
        State* s2 = new State();

        s0->connectTo(s1, 'a', 'a', 1);
        s1->connectTo(s2, 'b', 'b', 1);
        s2->connectTo(s0, 'c', 'c', 1);

        t1.addState(s0);
        t1.addState(s1);
        t1.addState(s2);

        t1.setInitialState(s0);
        t1.visualize(std::cout);
    }

    Transducer t2;

    {
        State* s0 = new State();

        s0->connectTo(s0, 'a', 'b', 1);
        s0->connectTo(s0, 'b', 'a', 1);
        s0->connectTo(s0, 'c', 'c', 1);

        t2.addState(s0);

        t2.setInitialState(s0);
        t2.visualize(std::cout);
    }

    Transducer t = t1.composition(t2);

    t.visualize(std::cout);

    return 0;
}
