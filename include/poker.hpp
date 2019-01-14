#ifndef _POKER_HPP_
#define _POKER_HPP_

#include <string>

enum Suit { DIAMONDS=0, CLUBS=1, HEARTS=2, SPADES=3, JOKER=4 };
std::string str( Suit suit );

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

struct Card {
  Suit suit;
  Rank rank;
};

constexpr bool operator== ( const Card& a, const Card& b ) noexcept {
  return a.suit == b.suit && a.rank == b.rank;
}

constexpr bool operator!=( const Card& a, const Card& b ) noexcept {
  return !( a == b );
}


#endif
