#include "icu.hpp"
#include <iostream>

string_type convertUnicode(const icu::UnicodeString& ustr)
{
    auto it = StringCharacterIterator(ustr);

    string_type s;
    while ( it.hasNext() )
    {
        auto x = it.next32();
        s.push_back(x);
        std::cout << "Debug: " << x << '\n';
    }

    return s;
}
