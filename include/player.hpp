#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "referee.hpp"
#include "cardstream.hpp"
#include <functional>

using strategy_t = std::function<std::vector<Card>()>;

class Player : private cardstream {
private:
  const Declaration& _dcl;
  Referee _ref;
  strategy_t _strategy; // TODO implement this Human and AIs

  virtual std::vector<Card> select_cards() override final;

public:
  Player( const Declaration& dcl, strategy_t strategy )
    : _dcl(dcl), _strategy( std::move(strategy) ) {}
};

#endif
