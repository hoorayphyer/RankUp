#ifndef _TITLE_HPP_
#define _TITLE_HPP_

#include "poker.hpp"

// a title is a modification to suit that takes into account the lordship.
enum Title {
  FOLK_DIAMONDS = DIAMONDS,
  FOLK_CLUBS = CLUBS,
  FOLK_HEARTS = HEARTS,
  FOLK_SPADES = SPADES,
  LORD = JOKER
};

std::string str( Title title ) {
  if ( LORD == title )
    return "Lord";
  else
    return "Folk of " + str(static_cast<Suit>(title));
}

#endif
