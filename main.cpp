#include <iostream>
#include <fstream>
#include "transducer.hpp"
#include "intervalunion.hpp"

int main()
{
    
    Transducer X;

    std::ifstream in1("x.trans");

    X.readFromFile(in1);
    X.addEpsilonTransitions();
    X.visualize(std::cout);

    Transducer T;

    std::ifstream in2("t.trans");
    
    T.readFromFile(in2);
    T.visualize(std::cout);

//    Transducer A = Transducer::fromRegexp("a(ac|ac)*c");
//    Transducer A = Transducer::fromRegexp({'a', '(', 'a', 'c', '|', 'a', 'c', ')', '*', 'c'});
//    A.visualize(std::cout);
    Transducer A;

    std::ifstream in3("a.trans");

    A.readFromFile(in3);
    A.visualize(std::cout);

//    Transducer U = X.composition(T).composition(A);
    Transducer XT = X.composition(T);
    XT.visualize(std::cout);
    auto U = XT.composition(A);

    U.visualize(std::cout);

    U.minWay();
    std::cout << "..............................";
    std::cout << '\n';

    return 0;
}
