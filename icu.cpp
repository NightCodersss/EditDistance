#include "icu.hpp"

string_type convertUnicode(icu::UnicodeString ustr)
{
    ustr.toLower();
    auto it = StringCharacterIterator(ustr);

    string_type s;
    
    it.setToStart();
    while ( it.hasNext() )
    {
        auto x = it.next32PostInc();
        s.push_back(x != EPS_char ? x : EPS);
    }

    return s;
}

icu::UnicodeString convertFromStringType(const string_type& str)
{
    auto ucs = icu::UnicodeString();
    for ( const auto& c : str )
        ucs.append(c);

    return ucs;
}
