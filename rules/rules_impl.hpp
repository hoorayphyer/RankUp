#pragma once
#include <cassert>

#include "rules.hpp"

namespace rankup {
class Value {
 public:
  Value(int8_t value, bool is_minor_lord = false,
        Suit minor_lord_suit = Suit::D) {
    m_data = (value << 3);
    if (is_minor_lord) {
      assert(minor_lord_suit != Suit::J);
      m_data += (static_cast<int8_t>(minor_lord_suit) << 1) + 1;
    }
  }

  int8_t full() const { return m_data; }

  int8_t major() const { return m_data >> 3; }

  int8_t minor() const { return (m_data >> 1) & 3; }

  bool is_minor_lord() const { return m_data & 1; }

  bool operator<(const Value& other) const { return full() < other.full(); }

 private:
  int8_t m_data;
};

struct Rules::RulesImpl {
 public:
  RulesImpl(const Card& lord) : m_lord(lord) {}

  virtual Value evaluate(const Card& card) const = 0;
  virtual bool is_lord(const Card& card) const = 0;

  /**
   * @param sorted_values, vector of values that are already sorted from low to
   * high
   * @return possibly any pair of additional minor lords
   */
  virtual std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values) const = 0;

 protected:
  Card m_lord;

  static std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values, const int8_t minor_lord_val,
      bool allow_adjacent_pair_to_the_left_of_minor_lords);
};

/**
 * lord.rank != wW, lord.suit != J
 */
class RulesLordful : public Rules::RulesImpl {
 public:
  using Rules::RulesImpl::RulesImpl;
  Value evaluate(const Card& card) const override;
  bool is_lord(const Card& card) const override;
  std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values) const override;

 private:
  static constexpr int8_t MINOR_LORD_VAL = 12;
};

/**
 *lord.rank != wW, lord.suit == J
 */
class RulesLordlessOverthrown : public Rules::RulesImpl {
 public:
  using Rules::RulesImpl::RulesImpl;
  Value evaluate(const Card& card) const override;
  bool is_lord(const Card& card) const override;
  std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values) const override;

 private:
  static constexpr int8_t MINOR_LORD_VAL = 12;
};

/**
 *lord.rank == wW, lord.suit == J
 */
class RulesLordlessRegular : public Rules::RulesImpl {
 public:
  using Rules::RulesImpl::RulesImpl;
  Value evaluate(const Card& card) const override;
  bool is_lord(const Card& card) const override;
  std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values) const override {
    // no actions
    return {};
  }
};

}  // namespace rankup