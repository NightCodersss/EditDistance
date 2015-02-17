#include "icu.hpp"

string_type convertUnicode(const icu::UnicodeString& ustr)
{
    auto it = StringCharacterIterator(ustr);

    string_type s;
    
    it.setToStart();
    while ( it.hasNext() )
    {
        auto x = it.next32PostInc();
        s.push_back(x);
    }

    return s;
}
