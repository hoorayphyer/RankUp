#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "deck.hpp"
#include "player.hpp"
#include "referee.hpp"

class Game {
private:
  Deck _deck;
  Player _player;
  Referee _referee;

public:
  void deal();
  void start();


};



#endif
