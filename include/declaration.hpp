#ifndef _DECLARATION_HPP_
#define _DECLARATION_HPP_

#include "title.hpp"
#include <unordered_map>

// TODO implement this struct. Use a data structure to reflect the components of some played cards under the following categories: single, pair, wagon ( Rui's choice of translation of tuo la ji )
struct Pattern {
  // length of each categories
  std::unordered_map<int,int> wagon;
  int pairs = 0;
  int singles = 0;
  float value = 0;
  float min_single = 0;
  float min_pair = 0;
  float min_wagon = 0;
  float max_single = 0;
  float max_pair = 0;
  float max_wagon = 0;

  bool operator== (const Pattern& other){
    return (this.pairs == other.pairs) && (this.singles == other.singles) && (this.wagon == other.wagon);
  };
};


// Declaration contains all information for referee to do all their work. This struct is meant as a pure dataset.
struct Declaration {
  Card lord;
  Pattern pattern;
};

#endif
