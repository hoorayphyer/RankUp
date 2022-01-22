#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "common/card.hpp"

namespace rankup {

class Format {
 public:
  explicit Format(Suit suit) : m_suit(suit) {}

  /**
     @return the reference to the count of the axle. If axle doesn't exist, an
     entry is created before returning its count whose value is initialized to
     0.
   */
  int8_t& operator[](const int8_t& axle);

  /**
     @return the reference to the count of the axle.
     @throw std::out_of_range if axle doesn't exist.
   */
  const int8_t& at(const int8_t& axle) const;
  int8_t& at(const int8_t& axle);

  /**
     @return the count of the axle if axle exists, and 0 otherwise
   */
  int8_t at_or_0(const int8_t& axle) const noexcept;

  const Suit& suit() const { return m_suit; }

  /**
     @return the total number of cards this format represents.
   */
  int8_t total_num_cards() const;

  /**
     @return whether `other` is compatible with this format. A compatible format
     has the same axle-count structure and has either the same suit or a lord
     suit denoted by Suit::J.
   */
  bool is_compatible(const Format& other) const;

 protected:
  Suit m_suit;

  // use two vectors to emulate a map from axle to its count. In practice, the
  // length of each vector is expected to be <= 3 most of the time.
  std::vector<int8_t> m_axle;
  std::vector<int8_t> m_count;

  inline static constexpr int INVALID_INDEX = -1;
  /**
     @return the index of `axle` in m_axle if existed, and INVALID_INDEX
     otherwise
   */
  int get_index(const int8_t& axle) const noexcept;
};

/**
   The composition of a set of cards with the same suit
 */
class Composition : private Format {
 public:
  friend class Rules;

  const Format& format() const { return static_cast<const Format&>(*this); }

 private:
  // keyed by Format::m_axle
  std::vector<std::vector<int8_t>> m_start;

  // minor lord components that must be separately stored
  struct ExtraMinorLordPairs {
    // assoc is the associated component of at least axle-3 that contains AA,
    // minor-minor, major-major
    int8_t m_assoc_start;
    int8_t m_assoc_axle;
    std::vector<int8_t> m_start;
  };

  std::optional<ExtraMinorLordPairs> m_extra;

  explicit Composition(Suit suit) : Format(suit) {}
};

class Pattern {
 public:
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

  /**
     When a pattern is present, check if the selected cards are valid to play

     @param cards, the entire cards of a player
     @param selected, a bit array of the same size as cards that specifies which
     cards are picked for playing

     @return empty string if valid, otherwise a string explaining invalidity
   */
  std::string check_valid(const std::vector<Card>& cards,
                          const std::vector<bool>& selected) const;

  enum class CompareCode : int8_t { OK = 0, DIFFERENT_COUNT, NO_SINGLE_SUIT };

  /**
     checks if `other` can be compared with this pattern. To be comparable, both
     patterns must have the same count(), and at least one must have a single
     suit.
   */
  CompareCode is_comparable(const Pattern& other) const;

  /**
     checks whether this pattern is beaten by `other`

     @throw std::runtime_error if `other` is not comparable to this pattern. See
     `is_comparable` for details
   */
  bool is_beaten_by(const Pattern& other) const;

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