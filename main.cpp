#include <iostream>
#include <fstream>
#include "transducer.hpp"

int main()
{
    std::cout << "OLOLO " << EPS << '\n';

    Transducer t1;

    std::ifstream in1("t1.trans");

    t1.readFromFile(in1);
    t1.visualize(std::cout);

    Transducer t2;

    std::ifstream in2("t2.trans");
    
    t2.readFromFile(in2);
    t2.visualize(std::cout);

    Transducer t = t1.composition(t2);

    t.visualize(std::cout);

    in1.close();
    in2.close();    

    return 0;
}
