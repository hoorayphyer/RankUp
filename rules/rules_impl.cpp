#include "rules_impl.hpp"

#include <algorithm>

namespace rankup {

Value RulesLordful::evaluate(const Card& card) const {
  static_assert(static_cast<char>(Rank::_2) == 0);
  char rank = static_cast<char>(card.rank());
  char lord_rank = static_cast<char>(m_lord.rank());

  if (card.suit() != Suit::J) {
    if (rank != lord_rank)
      return Value(rank > lord_rank ? rank : rank - 1);
    else if (card.suit() != m_lord.suit()) {
      return Value(12, true, card.suit());
    } else
      return Value(13);
  } else {
    return Value(card.rank() == Rank::_w ? 14 : 15);
  }
}

bool RulesLordful::is_lord(const Card& card) const {
  return card.suit() == m_lord.suit() or card.rank() == m_lord.rank() or
         card.suit() == Suit::J;
}

std::vector<Value> RulesLordful::adjust_for_minor_lords(
    std::vector<Value>& sorted_values) const {
  std::vector<Value> res;

  // TODO


}

Value RulesLordlessOverthrown::evaluate(const Card& card) const {
  static_assert(static_cast<char>(Rank::_2) == 0);
  char rank = static_cast<char>(card.rank());
  char lord_rank = static_cast<char>(m_lord.rank());

  if (card.suit() != Suit::J) {
    if (rank != lord_rank)
      return Value(rank > lord_rank ? rank : rank - 1);
    else
      return Value(12, true, card.suit());
  } else {
    return Value(card.rank() == Rank::_w ? 13 : 14);
  }
}

bool RulesLordlessOverthrown::is_lord(const Card& card) const {
  return card.rank() == m_lord.rank() or card.suit() == Suit::J;
}

std::vector<Value> RulesLordlessOverthrown::adjust_for_minor_lords(
    std::vector<Value>& sorted_values) const {
  std::vector<Value> res;

  // TODO
}

Value RulesLordlessRegular::evaluate(const Card& card) const {
  static_assert(static_cast<char>(Rank::_2) == 0);
  char rank = static_cast<char>(card.rank());

  return Value(rank);  // also works for wW
}

bool RulesLordlessRegular::is_lord(const Card& card) const {
  return card.suit() == Suit::J;
}
}  // namespace rankup