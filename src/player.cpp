#include "player.hpp"

std::vector<Card> Player::select_cards() {
  std::vector<Card> cards;
  auto& hand = _data;

  do { cards = _strategy(); }
  while ( !_ref.is_legal( cards, hand, _dcl ) );

  return cards;
}
