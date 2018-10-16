#ifndef _POKER_H_
#define _POKER_H_

#include <string>
#include <vector>

enum Suite { DIAMONDS=0, CLUBS=1, HEARTS=2, SPADES=3, JOKER=4 };
std::string str( Suite suite );

// _w is Joker low, and _W is Joker high
enum class Rank : char {
  _2='2', _3, _4, _5, _6, _7, _8, _9, _10, _J, _Q, _K, _A, _w, _W
};

std::string str( Rank rank );

template< typename OStream >
OStream& operator<< (OStream& out, Rank r) {
  out << str(r);
  return out;
}

struct Card{
  Suite suite;
  Rank rank;
};

inline bool operator==(const Card& a, const Card& b){
  return a.suite == b.suite && a.rank == b.rank;
}

inline bool operator!=(const Card& a, const Card& b){
  return !(a==b);
}

std::vector<Card> generate_deck();


#endif // ----- end of _POKER_H_
