#ifndef _PATTERN_H_
#define _PATTERN_H_

#include "poker.h"

// a title is a modification to suite that takes into account the lordship.
enum Title {
            FOLK_DIAMONDS = DIAMONDS,
            FOLK_CLUBS = CLUBS,
            FOLK_HEARTS = HEARTS,
            FOLK_SPADES = SPADES,
            LORD = JOKER
};

std::string str( Title title );

namespace titles {
  void set_lordship( const Card& card );
  const Card& query_lordship();
  Title title( const Card& card );
}

class Pattern {
  // TODO implement this
private:

public:
};

#endif // ----- end of _PATTERN_H_
