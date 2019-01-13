#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "poker.hpp"

// interface class for a player object
class Player {
private:
  std::vector<Card> _hand;
public:
  Player() { _hand.reserve( (54 * 2 - 8) / 4 ); }

  void draw( Card&& card ) { _hand.emplace_back(std::move(card) ); }

  // TODO what should be the signiture?
  virtual std::vector<Card> play() = 0;

  // TODO what should be the signiture?
  virtual void claim() = 0;

};

#endif
