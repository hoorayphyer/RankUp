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
  Format() = default;
  explicit Format(Suit suit) : m_suit(suit) {}

  /**
     @return the reference to the count of the axle. If axle doesn't exist, an
     entry is created before returning its count whose value is initialized to
     0.

     @throw std::runtime_error if Format doesn't have a suit, i.e. if Format is
     empty.
   */
  int8_t& get_count(const int8_t& axle);

  /**
     @return the reference to the count of the axle.
     @throw std::out_of_range if axle doesn't exist.
   */
  const int8_t& get_count_at(const int8_t& axle) const;
  int8_t& get_count_at(const int8_t& axle);

  /**
     @return the count of the axle if axle exists, and 0 otherwise
   */
  int8_t get_count_at_or_0(const int8_t& axle) const noexcept;

  const std::optional<Suit>& suit() const { return m_suit; }

  /**
     @return the total number of cards this format represents.
   */
  int8_t total_num_cards() const;

  /**
     @return whether this format is covered by `other`, which requires that
     `other` has either the same suit or a lord suit denoted by Suit::J, and its
     axle structure can afford that of this format. NOTE that `other` can have
     more cards.
   */
  bool is_covered_by(const Format& other) const;

  Format extract_required_format_from(const Format& other) const;

 protected:
  std::optional<Suit> m_suit = {};

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

  /**
     @throw std::runtime_error if m_axle is empty.
   */
  // TODO is it possible to declare that Format will always have one card at
  // least?? Look at this again when everything is done.
  //
  // Well, there is value to have Format with empty Cards. See
  // RoundRules::get_required_format. An empty Format is covered by any format.
  //
  // If so, one should make suit() return an optional.
  int get_index_highest_axle() const;
};

/**
   The composition of a set of cards with the same suit
 */
class Composition : private Format {
 public:
  friend class Rules;

  const Format& format() const { return static_cast<const Format&>(*this); }

  /**
     @return whether this composition defeats `other`, with the format set by
     the former.

     @throw std::runtime_error if other has a different number of cards.
   */
  bool defeats(const Composition& other) const;

 private:
  class Starts {
   public:
    // TODO other methods
    const int8_t& greatest() const;

   private:
    mutable bool m_sorted = false;
    mutable std::vector<int8_t> m_data;
  };

  // keyed by Format::m_axle
  std::vector<Starts> m_start;

  // TODO I want to make Composition free of any minor lords complications. I
  // want to move these complications to other classes

  // // minor lord components that must be separately stored
  // struct ExtraMinorLordPairs {
  //   // assoc is the associated component of at least axle-3 that contains AA,
  //   // minor-minor, major-major
  //   int8_t m_assoc_start;
  //   int8_t m_assoc_axle;
  //   std::vector<int8_t> m_start;
  // };

  // std::optional<ExtraMinorLordPairs> m_extra;

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

class Rules;

/**
   RoundRules enforces rules when a Composition is specified.
 */
class RoundRules {
 public:
  RoundRules(const Rules& rules, Composition cmp)
      : m_rules(rules), m_fmt(cmp.format()), m_winning_cmp(std::move(cmp)) {}

  /**
     Based on the given RoundRules, scan the hand to find the required format of
     cards to play

     @param hand, the entire cards of a player
     @return the required format of cards to be played from the hand
   */
  Format get_required_format(const std::vector<Card>& hand) const;

  /**
     @return true if the current composition is defeated by cards, and false
     otherwise.

     @throw std::runtime_error if cards has a different total number.
   */
  bool update_if_defeated_by(const std::vector<Card>& cards);

 private:
  const Rules& m_rules;
  const Format m_fmt;
  // NOTE m_winning_cmp may have a different suit than the original format, but
  // must have the same components.
  Composition m_winning_cmp;
};

class Rules {
 public:
  explicit Rules(Card lord_card);

  ~Rules();

  /**
     @param hand, the entire cards of a player
     @param selected, a bit array of the same size as cards that specifies
     which cards are picked for playing

     @return empty string if valid, otherwise a string explaining invalidity
   */
  std::string check_valid_as_first_cards(
      const std::vector<Card>& hand, const std::vector<bool>& selected) const;

  /**
     @return a RoundRules instance with `cards`.

     @throw std::runtime_error if cards i empty or cards doesn't have a uniform
     suit.
   */
  RoundRules start_round_with(const std::vector<Card>& cards) const;

  struct RulesImpl;

  friend class RoundRules;

 private:
  Card m_lord_card;

  std::unique_ptr<RulesImpl> m_impl;

  // keyed by Format::m_axle, and the vector of each axle is sorted from low to
  // high
  std::vector<std::vector<int8_t>> m_start;

  // minor lord components that must be separately stored
  struct ExtraMinorLordPairs {
    // assoc is the associated component of at least axle-3 that contains AA,
    // minor-minor, major-major
    //
    // TODO maybe we just need std::vector<int8_t> start. The other two can be
    // reconstructed on the fly. Although it can be costly, the chances of
    // having to do that in real life is extremely low. So we are good. The good
    // part is that we can get rid of this ExtraMinorLordPairs class.
    int8_t assoc_start = 0;
    int8_t assoc_axle = 0;
    std::vector<int8_t> start;
  };

  struct EnhancedComposition {
    Composition cmp;
    ExtraMinorLordPairs extra;

    bool empty_minor_lord_pairs() const { return extra.start.empty(); }

    // the following are two ways of absorbing extra into cmp to make a single
    // Composition representation. They return the newly constructed Composition
    // objects by first making a copy of cmp. This is justified because these
    // functions are very rarely used.
    Composition direct_append_extra() const;
    Composition split_merge_extra() const;
  };

  /**
     @return EnhancedComposition object if cards have the same suit, and nullopt
     otherwise indicating the cards have more than one suit.

     @throw std::runtime_error if cards is empty.
   */
  std::optional<EnhancedComposition> parse_for_single_suit(
      const std::vector<Card>& cards) const;
};

}  // namespace rankup