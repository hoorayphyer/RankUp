#ifndef _DECLARATION_HPP_
#define _DECLARATION_HPP_

#include "title.hpp"

// TODO implement this struct. Use a data structure to reflect the components of some played cards under the following categories: single, pair, wagon ( Rui's choice of translation of tuo la ji )
struct Pattern {

};

// Declaration contains all information for referee to do all their work. This struct is meant as a pure dataset.
struct Declaration {
  Card lord;
  Pattern pattern;
};

#endif
