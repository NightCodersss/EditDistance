#include <iostream>
#include <fstream>
#include "transducer.hpp"

int main()
{
    /*
    Transducer X;

    std::ifstream in1("x.trans");

    X.readFromFile(in1);
    X.addEpsilonTransitions();
    X.visualize(std::cout);

    Transducer T;

    std::ifstream in2("t.trans");
    
    T.readFromFile(in2);
    T.visualize(std::cout);

    Transducer A;

    std::ifstream in3("a.trans");

    A.readFromFile(in3);
    A.visualize(std::cout);

    Transducer U = X.composition(T).composition(A);

    U.visualize(std::cout);

    in1.close();
    in2.close();    
    in3.close();

    U.minWay();
    std::cout << "..............................";
    std::cout << '\n';
*/

    Transducer t = Transducer::fromRegexp("1|2*");
    t.visualize(std::cout);

    return 0;
}
