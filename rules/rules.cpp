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

namespace parse_impl {
/**
   @param cards, guaranteed to have size > 0
 */
std::tuple<bool, int8_t, Suit, std::vector<Value>> construct_sorted_values(
    const std::vector<Card>& cards, const Rules::RulesImpl* impl) {
  bool mixed = false;
  int8_t count = 0;
  Suit suit;
  std::vector<Value> values;
  values.reserve(cards.size());

  auto lorded_suit = [impl](const Card& card) {
    // Here we use Suit::J to represent all lords
    return impl->is_lord(card) ? Suit::J : card.suit();
  };

  suit = lorded_suit(cards[0]);
  values.push_back(impl->evaluate(cards[0]));
  for (auto i = 1u; i < cards.size(); ++i) {
    if (lorded_suit(cards[i]) != suit) {
      mixed = true;
      break;
    }
    values.push_back(impl->evaluate(cards[i]));
  }

  if (!mixed) {
    count = cards.size();
    std::sort(values.begin(), values.end());
  }

  return {mixed, count, suit, values};
}
}  // namespace parse_impl

Pattern Rules::parse(const std::vector<Card>& cards) const {
  if (cards.size() == 0) {
    throw std::runtime_error("Parsing empty cards is forbidden!");
  }
  Pattern pat;
  std::vector<Value> values;

  std::tie(pat.m_mixed, pat.m_count, pat.m_suit, values) =
      parse_impl::construct_sorted_values(cards, m_impl.get());
  if (pat.m_mixed) return pat;

  auto extra_minor_lord_pairs = m_impl->adjust_for_minor_lords(values);
}
}  // namespace rankup