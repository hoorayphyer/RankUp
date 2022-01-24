#include "rules/rules.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "common/definitions.hpp"
#include "rules_impl.hpp"

namespace rankup {
int Format::get_index(const int8_t& axle) const noexcept {
  auto it = std::find(m_axle.begin(), m_axle.end(), axle);
  return it != m_axle.end() ? std::distance(m_axle.begin(), it) : INVALID_INDEX;
}

int8_t& Format::operator[](const int8_t& axle) {
  auto idx = get_index(axle);
  if (idx != INVALID_INDEX) {
    return m_count[idx];
  } else {
    m_axle.push_back(axle);
    return m_count.emplace_back(0);
  }
}

const int8_t& Format::count_at(const int8_t& axle) const {
  auto idx = get_index(axle);
  if (idx == INVALID_INDEX) {
    std::string msg = "Axle " + std::to_string(static_cast<int>(axle)) +
                      " not found in the format!";
    throw std::out_of_range(msg);
  }
  return m_count[idx];
}

int8_t& Format::count_at(const int8_t& axle) {
  return const_cast<int8_t&>(static_cast<const Format&>(*this).count_at(axle));
}

int8_t Format::count_at_or_0(const int8_t& axle) const noexcept {
  auto idx = get_index(axle);
  return idx != INVALID_INDEX ? m_count[idx] : 0;
}

int8_t Format::total_num_cards() const {
  int8_t res = 0;
  for (auto i = 0u; i < m_axle.size(); ++i) {
    res += m_count[i] * (m_axle[i] == 0 ? 1 : 2 * m_axle[i]);
  }
  return res;
}

bool Format::is_compatible(const Format& other) const {
  if (other.suit() != m_suit and other.suit() != Suit::J) return false;

  if (m_axle.size() != other.m_axle.size()) return false;

  // I use a N^2 algorithm here as the vector sizes are small
  for (auto i = 0u; i < m_axle.size(); ++i) {
    if (other.count_at_or_0(m_axle[i]) != m_count[i]) return false;
  }
  return true;
}
}  // namespace rankup

namespace rankup {
bool Composition::defeats(const Composition& other) const {
  if (total_num_cards() != other.total_num_cards()) {
    throw std::runtime_error(
        "Composition::defeats called with mismatched total number of cards!");
  }

  if (suit() == other.suit()) {
    if (is_compatible(other)) {
      // this defeats `other` by having the greatest of ANY ONE type of axle >=
      // that in `other`.
      auto f_max_start = [](const Composition& cps, int8_t axle) {
        auto idx = cps.get_index(axle);
        // NOTE m_start is already sorted
        return cps.m_start[idx].back();
      };

      for (const auto& axle : m_axle) {
        const auto& max_start = f_max_start(*this, axle);
        const auto& max_start_other = f_max_start(other, axle);
        if (max_start >= max_start_other) return true;
      }
      return false;
    } else {
      // other's axle structure doesn't match, so it loses
      return true;
    }
  } else if (suit() == Suit::J) {
    // other isn't composed of lords, so always loses
    return true;
  } else if (other.suit() == Suit::J) {
    // check if other has the same format
    return is_compatible(other);
  } else {
    // other has folks of a different suit than this, so always loses
    return true;
  }
}
}  // namespace rankup

namespace rankup {
std::string RoundRules::check_valid_for_entire_hand(
    const std::vector<Card>& hand, const std::vector<bool>& selected) const {
  // TODO implement
}

bool RoundRules::update_if_defeated_by(const std::vector<Card>& cards) {
  // TODO implement
}
}  // namespace rankup

namespace rankup {
std::string Rules::check_valid_as_first_cards(
    const std::vector<Card>& hand, const std::vector<bool>& selected) const {
  // TODO implement
}

RoundRules Rules::start_round_with(const std::vector<Card>& cards) const {
  // TODO implement
}
}  // namespace rankup

namespace rankup {
Pattern::CompareCode Pattern::is_comparable(const Pattern& other) const {
  if (count() != other.count()) return CompareCode::DIFFERENT_COUNT;
  return (single_suit() or other.single_suit()) ? CompareCode::OK
                                                : CompareCode::NO_SINGLE_SUIT;
}

bool Pattern::is_beaten_by(const Pattern& other) const {
  switch (is_comparable(other)) {
    case CompareCode::DIFFERENT_COUNT:
      throw std::runtime_error("not comparable because counts are different!");
    case CompareCode::NO_SINGLE_SUIT:
      throw std::runtime_error(
          "not comparable because neither has a single suit!");
    default:;
  };

  // TODO
}
}  // namespace rankup

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
std::tuple<bool, Suit, std::vector<Value>> construct_sorted_values(
    const std::vector<Card>& cards, const Rules::RulesImpl* impl) {
  bool single_suit = true;
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
      single_suit = false;
      break;
    }
    values.push_back(impl->evaluate(cards[i]));
  }

  if (single_suit) {
    std::sort(values.begin(), values.end());
  }

  return {single_suit, suit, values};
}

/**
   @param values, values are sorted, free of complications due to minor lords.
 */
std::vector<Pattern::Component> parse_for_components(
    const std::vector<Value>& values) {
  std::vector<Pattern::Component> res;
  if (values.empty()) return res;

  // define two modes for parsing.
  constexpr bool FOR_EQUAL = false;
  constexpr bool FOR_ADJ = true;

  // NOTE in storing the m_start of a Component, we store Value::major() as
  // opposed to full().

  Value val_last = values[0];
  int8_t axle = 0;
  bool mode = FOR_EQUAL;

  auto parse_in_equal_mode = [&val_last, &axle, &mode, &res](const Value& val) {
    // in comparing equal for a pair, we must compare full().
    if (val.full() == val_last.full()) {
      ++axle;
      mode = FOR_ADJ;
    } else {
      // fail to pair.
      if (axle != 0) {
        res.emplace_back(axle, val_last.major() - axle);
      }
      res.emplace_back(0, val_last.major());
      val_last = val;
      axle = 0;  // reset
    }
  };

  auto parse_in_adj_mode = [&val_last, &axle, &mode, &res](const Value& val) {
    //  in comparing adj, we must compare major().
    if (val.major() == val_last.major() + 1) {
      val_last = val;
      mode = FOR_EQUAL;
    } else {
      res.emplace_back(axle, val_last.major() - axle + 1);
      val_last = val;
      axle = 0;
      mode = FOR_EQUAL;
    }
  };

  // conceptually append a sentinel to `values` to make sure the last element in
  // `values` is stored.
  const Value sentinel(std::numeric_limits<int8_t>::max());
  for (auto i = 1u; i <= values.size(); ++i) {
    const auto& val = i != values.size() ? values[i] : sentinel;
    if (mode == FOR_EQUAL)
      parse_in_equal_mode(val);
    else
      parse_in_adj_mode(val);
  }

  return res;
}

}  // namespace parse_impl

Pattern Rules::parse(const std::vector<Card>& cards) const {
  if (cards.size() == 0) {
    throw std::runtime_error("Parsing empty cards is forbidden!");
  }
  Pattern pat;
  std::vector<Value> values;

  std::tie(pat.m_single_suit, pat.m_suit, values) =
      parse_impl::construct_sorted_values(cards, m_impl.get());
  if (!pat.m_single_suit) return pat;
  pat.m_count = cards.size();

  auto extra_minor_lord_pairs = m_impl->adjust_for_minor_lords(values);

  pat.m_comps = parse_impl::parse_for_components(values);
  pat.m_minor_lord_comps =
      parse_impl::parse_for_components(extra_minor_lord_pairs);

  return pat;
}
}  // namespace rankup