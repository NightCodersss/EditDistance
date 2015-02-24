#ifndef _ICU_HPP_
#define _ICU_HPP_

#include <unicode/unistr.h>
#include <unicode/schriter.h>
#include <unicode/ustream.h>
#include "chartype.hpp"

using namespace icu_52;

string_type convertUnicode(icu::UnicodeString ustr);
icu::UnicodeString convertFromStringType(const string_type& str);

#endif
