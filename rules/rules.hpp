#pragma once

#include <cstdint>
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

  struct Component {
    Component(int8_t axle, int8_t start) : m_axle(axle), m_start(start) {}

    const int8_t& axle() const { return m_axle; }
    const int8_t& start() const { return m_start; }

    bool operator==(const Component& other) const {
      return axle() == other.axle() and start() == other.start();
    }

   private:
    int8_t m_axle;   // use axle number to represent component
    int8_t m_start;  // the value of the starting card
  };

  enum class CompareCode : int8_t { OK = 0, DIFFERENT_COUNT, NO_SINGLE_SUIT };

  /**
     checks if `other` can be compared with this pattern. To be comparable, both
     patterns must have the same count(), and at least one must have a single
     suit.
   */
  CompareCode is_comparable(const Pattern& other) const;

  /**
     checks whether this pattern is strictly smaller than other.

     @throw std::runtime_error if `other` is not comparable to this pattern. See
     `is_comparable` for details
   */
  bool operator<(const Pattern& other) const;
  bool operator>=(const Pattern& other) const { return !(*this < other); }
  bool operator>(const Pattern& other) const { return other < *this; }
  bool operator<=(const Pattern& other) const { return other >= *this; }

  const bool& single_suit() const { return m_single_suit; }
  const int8_t& count() const { return m_count; }
  const Suit& suit() const { return m_suit; }
  const std::vector<Component>& comps() const { return m_comps; }
  const std::vector<Component>& minor_lord_comps() const {
    return m_minor_lord_comps;
  }

  friend class Rules;

 private:
  Pattern() = default;

  bool m_single_suit = true;
  int8_t m_count = 0;  // number of cards of this pattern
  Suit m_suit;
  std::vector<Component> m_comps;
  std::vector<Component> m_minor_lord_comps;  // minor lord components that must
                                              // be separately stored
};

template <typename OStream>
OStream& operator<<(OStream& o, const Pattern::Component& comp) {
  o << static_cast<int>(comp.axle()) << "-" << static_cast<int>(comp.start());
  return o;
}

class Rules {
 public:
  explicit Rules(Card lord_card);

  ~Rules();

  /**
     @throw std::runtime_error if cards is empty.
   */
  Pattern parse(const std::vector<Card>& cards) const;

  struct RulesImpl;

 private:
  Card m_lord_card;

  std::unique_ptr<RulesImpl> m_impl;
};

}  // namespace rankup