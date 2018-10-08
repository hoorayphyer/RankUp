#include "title.h"

std::string str( Title title ) {
  if ( LORD == title )
    return "Lord";
  else
    return "Folk of " + str(static_cast<Suite>(title));
}

namespace titles {
  Card lord_card = {JOKER, Rank::_W};

  void set_lordship( const Card& card ) { lord_card = card; }
  const Card& query_lordship() { return lord_card; }

  bool is_lord( const Card& card ) {
    return ( card.suite == JOKER || card.suite == lord_card.suite )
      || ( card.rank == lord_card.rank );
  }

  Title title( const Card& card ) {
    if ( is_lord(card) )
      return LORD;
    else
      return static_cast<Title>(card.suite);
  }

}
