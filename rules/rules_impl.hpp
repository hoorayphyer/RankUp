#pragma once
#include <cassert>

#include "rules.hpp"

namespace rankup {
class Value {
 public:
  Value(char value, bool is_minor_lord = false,
        Suit minor_lord_suit = Suit::D) {
    m_data = (value << 3);
    if (is_minor_lord) {
      assert(minor_lord_suit != Suit::J);
      m_data += (static_cast<char>(minor_lord_suit) << 2) + 1;
    }
  }

  char full() const { return m_data; }

  char major() const { return m_data >> 3; }

  char minor() const { return (m_data >> 1) & 3; }

  bool is_minor_lord() const { return m_data & 1; }

  bool operator<(const Value& other) const { return full() < other.full(); }

 private:
  char m_data;
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
  static constexpr char MINOR_LORD_VAL = 12;
};

/**
 *lord.rank != wW, lord.suit == J
 */
class RulesLordlessOverthrown : public Rules::RulesImpl {
  using Rules::RulesImpl::RulesImpl;
  Value evaluate(const Card& card) const override;
  bool is_lord(const Card& card) const override;
  std::vector<Value> adjust_for_minor_lords(
      std::vector<Value>& sorted_values) const override;

 private:
  static constexpr char MINOR_LORD_VAL = 12;
};

/**
 *lord.rank == wW, lord.suit == J
 */
class RulesLordlessRegular : public Rules::RulesImpl {
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