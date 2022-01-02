#include <algorithm>
#include <stdexcept>

#include "rules_impl.hpp"

namespace rankup {

Rules::Rules(Card lord_card) : m_lord_card(std::move(lord_card)) {
  // Three cases of lordedness
  //
  // 1. lord.rank != wW, lord.suit != J
  //
  // 2. lord.rank != wW, lord.suit == J
  //
  // 3. lord.rank == wW, lord.suit == J

  if (m_lord_card.rank() < Rank::_w) {
    if (m_lord_card.suit() != Suit::J) {
      m_impl.reset(new RulesLordful(m_lord_card));
    } else {
      m_impl.reset(new RulesLordlessOverthrown(m_lord_card));
    }
  } else {
    m_impl.reset(new RulesLordlessRegular(m_lord_card));
  }
}

Rules::~Rules() = default;

Pattern Rules::parse(const std::vector<Card> &cards) const {
  if (cards.size() == 0) {
    throw std::runtime_error("Parsing empty cards is forbidden!");
  }
  Pattern pat;
  std::vector<Value> values;
  values.reserve(cards.size());

  auto lorded_suit = [this](const Card &card) {
    // Here we use Suit::J to represent all lords
    return m_impl->is_lord(card) ? Suit::J : card.suit();
  };

  pat.m_suit = lorded_suit(cards[0]);
  values.push_back(m_impl->evaluate(cards[0]));
  for (auto i = 1u; i < cards.size(); ++i) {
    if (lorded_suit(cards[i]) != pat.m_suit) {
      pat.m_mixed = true;
      return pat;
    }
    values.push_back(m_impl->evaluate(cards[i]));
  }
  pat.m_mixed = false;
  pat.m_count = cards.size();

  std::sort(values.begin(), values.end());

  auto extra_minor_lord_pairs = m_impl->adjust_for_minor_lords(values);
}
}  // namespace rankup