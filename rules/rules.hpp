#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/card.hpp"

namespace rankup {

class Pattern {
 public:
  /**
     When a pattern is present, check if the selected cards are valid to play

     @param cards, the entire cards of a player
     @param selected, a bit array of the same size as cards that specifies which
     cards are picked for playing

     @return empty string if valid, otherwise a string explaining invalidity
   */
  std::string check_valid(const std::vector<Card>& cards,
                          const std::vector<bool>& selected) const;

  friend class Rules;

 private:
  Pattern() = default;

  struct Component {
    int8_t m_axle;   // use axle number to represent component
    int8_t m_start;  // the value of the starting card
  };

  bool m_mixed = false;
  int8_t m_count = 0;  // number of cards of this pattern
  Suit m_suit;
  std::vector<Component> m_comps;
};

class Rules {
 public:
  Rules(Card lord_card);

  ~Rules();

  /**
     @throw std::runtime_error if cards is empty.
   */
  Pattern parse(const std::vector<Card>& cards) const;

  /**
     check if the set of cards is strictly greater than the pattern. If so,
     update the pattern to that of the cards.

     @return true if cards > pattern, false if cards <= pattern. In other words,
     return true if an update took place
   */
  bool update_pattern_if_cards_are_greater(
      Pattern& pattern, const std::vector<Card>& cards) const;

  struct RulesImpl;

 private:
  Card m_lord_card;

  std::unique_ptr<RulesImpl> m_impl;
};

}  // namespace rankup