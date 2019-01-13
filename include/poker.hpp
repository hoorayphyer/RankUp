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
private:
  Suit _suit;
  Rank _rank;
public:
  constexpr Card( Suit s, Rank r ) noexcept : _suit(s), _rank(r) {}

  constexpr Suit& suit() noexcept { return _suit;}
  constexpr const Suit& suit() const noexcept { return _suit; }
  constexpr Rank& rank() noexcept { return _rank;}
  constexpr const Rank& rank() const { return _rank; }

  constexpr bool operator== ( const Card& other ) noexcept {
    return _suit == other.suit() && _rank == other.rank();
  }

  constexpr bool operator!=( const Card& b ) noexcept {
    return !( a == b );
  }
};


#endif
