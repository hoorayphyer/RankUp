%module rankup

%{
#include "common/card.hpp"
#include "rules/rules.hpp"
%}

// class Card
%rename(equal) rankup::Card::operator==;
%ignore rankup::Card::operator!=;

// class Format
%rename(equal) rankup::Format::operator==;
%ignore rankup::Format::insert;
%ignore rankup::Format::get_count_at(int8_t const &);
%ignore rankup::Format::operator std::string() const;

// class Composition
%rename(equal) rankup::Composition::operator==;
%ignore rankup::Composition::insert;
%ignore rankup::Composition::get_start_map;
%ignore rankup::Composition::operator std::string() const;

// for int8_t. Note that stdint.i still issues warning(315) about std::int8_t
%include "stdint.i"
%include "common/definitions.hpp"
%include "common/card.hpp"
%include "rules/rules.hpp"