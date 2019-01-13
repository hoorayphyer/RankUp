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

std::string str( Title title );

namespace titles {
  void set_lordship( Suit s, Rank r );
  const Card& query_lordship();
  Title title( const Card& card );
}

#endif
