#include "poker.h"
#include <type_traits>

std::string str( Suite s ) {
  switch (s) {
  case DIAMONDS: return "Diamonds";
  case CLUBS: return "Clubs";
  case HEARTS: return "Hearts";
  case SPADES: return "Spades";
  default : return "JOKER";
  }
}

std::string str( Rank r ) {
  switch (r) {
  case Rank::_10 : return "10";
  case Rank::_J  : return "J";
  case Rank::_Q  : return "Q";
  case Rank::_K  : return "K";
  case Rank::_A  : return "A";
  case Rank::_w  : return "JOKER_LOW";
  case Rank::_W  : return "JOKER_HIGH";
  default:
    // using constructor std::string( size_t n, char c )
    return std::string( 1, static_cast<char>(r) );
  }
}

template< typename ENUM >
inline void operator++( ENUM& s ) {
  s = static_cast<ENUM>( static_cast<std::underlying_type_t<ENUM>>(s) + 1 );
}

std::vector<Card> generate_deck() {
  std::vector<Card> deck;
  for ( Suite s = DIAMONDS; s < JOKER; ++s ) {
    for ( Rank r = Rank::_2; r < Rank::_w; ++r ) {
      // NOTE Card here is necessary because template deduction cannot tell the type of a brack-initializer-list
      deck.emplace_back(Card{s,r});
    }
  }
  deck.emplace_back(Card{JOKER, Rank::_w});
  deck.emplace_back(Card{JOKER, Rank::_W});
  deck.shrink_to_fit();
  return deck;
}
