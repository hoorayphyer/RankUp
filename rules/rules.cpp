#include "rules.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace rankup {

// Three cases of lordship
//
// 1. lord.rank != wW, lord.suit != J
//
// 2. lord.rank != wW, lord.suit == J
//
// 3. lord.rank == wW, lord.suit == J
enum { LORDFUL, OVERTHROWN_LORDLESS, REGULAR_LORDLESS };

int get_lordship(const Card &lord) {
  if (lord.rank() < Rank::_w) {
    return lord.suit() != Suit::J ? LORDFUL : OVERTHROWN_LORDLESS;
  } else {
    return REGULAR_LORDLESS;
  }
}

Rules::Value::Value(char value, bool is_minor_lord, Suit minor_lord_suit) {
  m_data = (value << 3);
  if (is_minor_lord) {
    assert(minor_lord_suit != Suit::J);
    m_data += (static_cast<char>(minor_lord_suit) << 2) + 1;
  }
}

char Rules::Value::major() const { return m_data >> 3; }

char Rules::Value::minor() const { return (m_data >> 1) & 3; }

bool Rules::Value::is_minor_lord() const { return m_data & 1; }

Rules::Value Rules::evaluate(const Card &card) const {
  // We determine the value sequences for the lord cards. The sequence will
  // start from 0 and increase consecutively. The same sequence can be applied
  // to folk cards as well.
  //
  static_assert(static_cast<char>(Rank::_2) == 0);
  char rank = static_cast<char>(card.rank());
  char lord_rank = static_cast<char>(m_lord_card.rank());

  switch (get_lordship(m_lord_card)) {
    case LORDFUL:
      if (card.suit() != Suit::J) {
        if (rank != lord_rank)
          return Value(rank - (rank > lord_rank));
        else if (card.suit() != m_lord_card.suit()) {
          return Value(12, true, card.suit());
        } else
          return Value(13);
      } else {
        return Value(card.rank() == Rank::_w ? 14 : 15);
      }
    case OVERTHROWN_LORDLESS:
      if (card.suit() != Suit::J) {
        if (rank != lord_rank)
          return Value(rank - (rank > lord_rank));
        else
          return Value(12, true, card.suit());
      } else {
        return Value(card.rank() == Rank::_w ? 13 : 14);
      }
    case REGULAR_LORDLESS:
      return Value(rank);  // also works for wW
  };
}

bool Rules::is_lord(const Card &card) const {
  bool res = false;
  switch (get_lordship(m_lord_card)) {
    case LORDFUL:
      if (res |= (card.suit() == m_lord_card.suit())) return true;
    case OVERTHROWN_LORDLESS:
      if (res |= (card.rank() == m_lord_card.rank())) return true;
    case REGULAR_LORDLESS:
      return card.suit() == Suit::J;
  };
}

Pattern Rules::parse(const std::vector<Card> &cards) const {
  if (cards.size() == 0) {
    throw std::runtime_error("Parsing empty cards is forbidden!");
  }
  Pattern pat;
  std::vector<Value> values;
  values.reserve(cards.size());

  auto lorded_suit = [this](const Card &card) {
    // Here we use Suit::J to represent all lords
    return is_lord(card) ? Suit::J : card.suit();
  };

  pat.m_suit = lorded_suit(cards[0]);
  values.push_back(evaluate(cards[0]));
  for (auto i = 1u; i < cards.size(); ++i) {
    if (lorded_suit(cards[i]) != pat.m_suit) {
      pat.m_mixed = true;
      return pat;
    }
    values.push_back(evaluate(cards[i]));
  }
  pat.m_mixed = false;
  pat.m_count = cards.size();

  std::sort(values.begin(), values.end());
}
}  // namespace rankup