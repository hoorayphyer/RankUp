#ifndef _CHRONICLE_HPP_
#define _CHRONICLE_HPP_

#include "poker.hpp"

// RoundEvent stores what card or cards are played and who played first
// TODO: implement this
class RoundEvent {
private:

public:
  // return the card(s) played by that player
  const std::vector<Card>& operator[] ( int player_id );
};

// TODO: implement this. The data structure should be a queue + random access
class Chronicle {
private:
  std::vector<RoundEvent> _data;
public:
  const RoundEvent& operator[] ( int round ) {
    // convention: round is negative. -1 means last round.
    // users are responsible for making sure round < 0
    return _data[-round - 1];
  }

  void push( const RoundEvent& event );
  void pop();
};

#endif // ----- end of _CHRONICLE_H_
