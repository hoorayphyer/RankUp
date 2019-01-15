#ifndef _REFEREE_HPP_
#define _REFEREE_HPP_

#include "declaration.hpp"
#include <vector>

// TODO implement this whole class given the following interface
class Referee {
public:
  Pattern parse( const std::vector<Card>& cards );

  bool is_legal( const std::vector<Card>& cards,
                 const std::vector<Card>& hand,
                 const Declaration& dcl );

  bool compare( const std::vector<Card>& a, const std::vector<Card>& b,
                const Declaration& dcl );

  bool is_lord( const Card& card, const Declaration& dcl ) {
    return ( card.suit == JOKER || card.suit == dcl.lord.suit )
      || ( card.rank == dcl.lord.rank );
  }

};

#endif
