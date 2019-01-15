#ifndef _DECK_HPP_
#define _DECK_HPP_

#include "cardstream.hpp"

class Deck : private cardstream {
private:
  virtual std::vector<Card> select_cards() override final;

public:
  void initialize();
};

#endif
