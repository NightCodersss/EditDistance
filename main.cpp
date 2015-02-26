#include <iostream>
#include <fstream>
#include "transducer.hpp"
#include "intervalunion.hpp"

#include "icu.hpp"

int main()
{
/*    
    Transducer X;

    std::ifstream in1("x.trans");

    X.readFromFile(in1);
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

*/
/*
    std::ifstream in("test.trans");

    Transducer T;
    T.readFromFile(in);

    while ( true )
    {
        auto p = T.getNextMinPath();
        if ( p.cost == -1 )
            break;

        for ( const auto& state : p.path )
            std::cout << state -> id() << " ";
        std::cout << '\n';
    }
*/
/*
    std::string s = "ε";
    auto ucs = UnicodeString::fromUTF8(StringPiece(s.c_str()));
    auto str = convertUnicode(ucs);

    for ( auto i : str )
        std::cout << "Unicode str symbol " << i << '\n';
*/

//    auto t = Transducer::fromRegexp({949, 945});
/*
    Transducer test;
    std::ifstream in("test.trans");
    test.readFromFile(in);

    test.visualize(std::cout);

    Transducer test2;
    std::ifstream in2("test2.trans");
    test2.readFromFile(in2);
    
    test2.visualize(std::cout);

    test.composition(test2).visualize(std::cout);
*/

    auto t = Transducer::fromRegexp(convertUnicode(UnicodeString::fromUTF8(StringPiece("a{2, 2}"))));
    t.visualize(std::cout);

    return 0;
}
