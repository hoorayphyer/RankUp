#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_
#include <array>
#include <vector>

#include "poker.hpp"

class Player; // should be able to query umpire, and maybe history
class Table; // rule enforcer, count credit, determine winner and rankup, set pattern in each round

class Engine {
private:
  using PlayerID = int;
  std::array<Player, 4> _players;
  Table _table;

public:
  void add_player(Player);

  // draw phase: deal cards, claim lordship, overthrow lordship and reserve
  void execute_round() {
    // play phase:
    // one player play, umpire check and set pattern
    // other players play in turns, umpire enforces rule and decide winner, then set winner of this round, award credits for the team
    // start next round
  }
};

namespace infra {
  // any class that has .port() method can participate in communication
  struct Buffer;

  template < typename Src, typename Tgt >
  void send_cards_from_to( Src& src, Tgt& tgt, std::vector<Card> cards ) {
    // May need a proxy that does the actually communication
    src >> cards >> tgt;
  }
}

namespace prep {
  // this includes overthrow lordship. Need to keep
  // track of whether overthrow is legitimate
  void claim_lordship() {

  }

  void check_claim_requests() {

  }

  void deal_deck() {
    // first all devices synchronize seed, then each device maintains their own deck
    // will need communication lock
    auto deck = generate_deck;
    deck >> deck.next() >> players[0];
    check_claim_requests();
    deck >> deck.next() >> players[1];
    check_claim_requests();
    deck >> deck.next() >> players[2];
    check_claim_requests();
    deck >> deck.next() >> players[3];
    check_claim_requests();

    deck >> deck.remaining_cards() >> player_win;
    player_win >> player_win.reserve
  }

}

namespace game {
  // I wanted to let each device handle their own referee, then once a card is locally legal, it will be dealt to table and have other referees inspect and they may raise objection.
  // version for players
  void my_turn( ) {
    if ( is_my_turn(player0) ) {
      auto card = player0.play(); // referee check is inside

      // this may be called broadcast
      player0 >> card >> {buf_player1, buf_player2, buf_player3};
      bool passed = wait_for_other_referees();
      if ( !passed ) {
        card = illegal_card_callback();
      }
      player0 >> card >> buf_table;

    } else {
      buf_player0 >> card;
      if ( !referee.is_legal(card) ) {
        // signal to player0 it is not legal
      }
    }

  }

}






#endif
