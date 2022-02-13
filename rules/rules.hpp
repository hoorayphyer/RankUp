#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/card.hpp"
#include "common/definitions.hpp"

namespace rankup {

class Format {
 public:
  Format() = default;
  explicit Format(Suit suit) : m_suit(suit) {}

  /**
     Insert axle into the format. If axle doesn't exist, an
     entry is created whose count is initialized to 0.

     @return the index corresponding to axle

     @throw std::runtime_error if Format doesn't have a suit, i.e. if Format is
     empty.
   */
  int8_t insert(int8_t axle);

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
  explicit Composition(Suit suit) : Format(suit) {}

  /**
     @return the index corresponding to axle
   */
  int8_t insert(int8_t axle, int8_t start);

  const Format& format() const { return static_cast<const Format&>(*this); }

  Suit suit() const { return *format().suit(); }

  auto total_num_cards() const { return format().total_num_cards(); }

  /**
     @return whether this composition defeats `other`, with the format set by
     the former.

     @throw std::runtime_error if other has a different number of cards.
   */
  bool defeats(const Composition& other) const;

  /**
     This is an ad hoc funtion to be used in
     Rules::EnhancedComposition::split_merge_extra().

     @return map with axle as the key and vector of starts as the value
   */
  std::unordered_map<int8_t, std::vector<int8_t>> get_start_map() const;

  bool operator==(const Composition& other) const;

  explicit operator std::string() const;

 private:
  class Starts {
   public:
    void insert(int8_t start);
    const int8_t& greatest() const;
    const auto& data() const { return m_data; }

    bool operator!=(const Starts& other) const;
    bool operator==(const Starts& other) const { return !(*this != other); }

   private:
    mutable bool m_sorted = false;
    mutable std::vector<int8_t> m_data;

    const Starts& sorted() const;
  };

  // keyed by Format::m_axle
  std::vector<Starts> m_start;
};

template <typename OStream>
OStream& operator<<(OStream& o, const Composition& cmp) {
  o << static_cast<std::string>(cmp);
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
  friend class TestRules;

 private:
  Card m_lord_card;

  std::unique_ptr<RulesImpl> m_impl;

  // keyed by Format::m_axle, and the vector of each axle is sorted from low to
  // high
  std::vector<std::vector<int8_t>> m_start;

  struct EnhancedComposition {
    EnhancedComposition(Composition c, std::vector<int8_t> extra)
        : cmp(std::move(c)), extra_ml_pair_start(std::move(extra)) {}

    Composition cmp;
    std::vector<int8_t> extra_ml_pair_start;

    bool empty_minor_lord_pairs() const { return extra_ml_pair_start.empty(); }

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