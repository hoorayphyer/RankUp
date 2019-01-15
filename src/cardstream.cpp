#include "cardstream.hpp"

std::vector<Card>& cardstream::operator>> ( std::vector<Card>& c ) {
  auto&& cards = select_cards();
  // remove elements in _buffer from _data TODO implement this
  // _data -= cards;
  c = std::move(cards);
  return c;
}
