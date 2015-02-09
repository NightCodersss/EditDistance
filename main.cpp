#include <iostream>
#include <fstream>
#include "transducer.hpp"
#include "intervalunion.hpp"

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

//    Transducer A = Transducer::fromRegexp("a(ac|ac)*c");
    Transducer A = Transducer::fromRegexp({'a', '(', 'a', 'c', '|', 'a', 'c', ')', '*', 'c'});
//    Transducer A;

//    std::ifstream in3("a.trans");

//    A.readFromFile(in3);
//    A.visualize(std::cout);

    Transducer U = X.composition(T).composition(A);

    U.visualize(std::cout);

    U.minWay();
    std::cout << "..............................";
    std::cout << '\n';
*/

    IntervalUnion iu1(false, { {0, 2}, {4, 6}, {8, 12}, {14, 17} });
    IntervalUnion iu2(false, { {1, 3}, {5, 9}, {10, 11}, {13, 14}, {16, 18} });
//    IntervalUnion iu1(false, { {1, 2} });
//    IntervalUnion iu2(false, { {2, 3} });

    iu1 = iu1.changeComplement();

    std::cout << "Interval 1:\n" << iu1.toString();        
    std::cout << "Interval 2:\n" << iu2.toString();        
    std::cout << "Intersection:\n" << iu1.intersection(iu2).toString();        
    std::cout << "Union:\n" << iu1.unite(iu2).toString();        

    return 0;
}
