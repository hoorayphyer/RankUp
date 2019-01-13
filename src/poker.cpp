#include "poker.hpp"

std::string str( Suit s ) {
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
  default: return std::string( 1, static_cast<char>(r) );
  }
}

// std::vector<Card> generate_deck() {
//   std::vector<Card> deck;
//   auto gen_suit =
//     [&deck]( Suit s ) {
//       for ( Rank r = Rank::_2; r < Rank::_w; ++r ) {
//         deck.emplace_back(s,r);
//       }
//     };
//   gen_suit(DIAMONDS);
//   gen_suit(CLUBS);
//   gen_suit(HEARTS);
//   gen_suit(SPADES);
//   deck.emplace_back(Card{JOKER, Rank::_w});
//   deck.emplace_back(Card{JOKER, Rank::_W});
//   deck.shrink_to_fit();
//   return deck;
// }
