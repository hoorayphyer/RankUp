#include "rules/rules.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>  // priority_queue
#include <stdexcept>
#include <type_traits>

#include "common/definitions.hpp"
#include "rules_impl.hpp"

namespace rankup {
int Format::get_index(const int8_t& axle) const noexcept {
  auto it = std::find(m_axle.begin(), m_axle.end(), axle);
  return it != m_axle.end() ? std::distance(m_axle.begin(), it) : INVALID_INDEX;
}

int Format::get_index_highest_axle() const {
  if (m_axle.empty())
    throw std::runtime_error(
        "No cards exist when calling get_index_highest_axle");
  auto it = std::max_element(m_axle.begin(), m_axle.end());
  return std::distance(m_axle.begin(), it);
}

int8_t Format::insert(int8_t axle) {
  if (!m_suit)
    throw std::runtime_error("Format::insert called in an empty format!");

  auto idx = get_index(axle);
  if (idx != INVALID_INDEX) {
    ++m_count[idx];
    return idx;
  } else {
    m_axle.push_back(axle);
    m_count.emplace_back(1);
    return m_axle.size() - 1;
  }
}

const int8_t& Format::get_count_at(const int8_t& axle) const {
  auto idx = get_index(axle);
  if (idx == INVALID_INDEX) {
    std::string msg = "Axle " + std::to_string(static_cast<int>(axle)) +
                      " not found in the format!";
    throw std::out_of_range(msg);
  }
  return m_count[idx];
}

int8_t& Format::get_count_at(const int8_t& axle) {
  return const_cast<int8_t&>(
      static_cast<const Format&>(*this).get_count_at(axle));
}

int8_t Format::get_count_at_or_0(const int8_t& axle) const noexcept {
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

bool Format::is_covered_by(const Format& other) const {
  if (!suit())
    return true;
  else if (!other.suit())
    return false;

  if (*other.suit() != *m_suit and *other.suit() != Suit::J) return false;

  auto ind_ax_max = std::distance(
      m_axle.begin(), std::max_element(m_axle.begin(), m_axle.end()));

  // TODO not finished yet.

  if (m_axle.size() != other.m_axle.size()) return false;

  // I use a N^2 algorithm here as the vector sizes are small
  for (auto i = 0u; i < m_axle.size(); ++i) {
    if (other.get_count_at_or_0(m_axle[i]) != m_count[i]) return false;
  }
  return true;
}

Format Format::extract_required_format_from(const Format& other) const {
  if (!suit() or !other.suit()) return {};

  assert(*suit() == *other.suit());

  assert(total_num_cards() > 0);
  assert(other.total_num_cards() > 0);

  auto make_pq = [](const Format& f) {
    std::priority_queue<int8_t> res;
    for (int i = 0; i < f.m_axle.size(); ++i) {
      // push m_ax by m_count times
      for (int c = 0; c < f.m_count[i]; ++c) {
        res.push(f.m_axle[i]);
      }
    }
    return res;
  };

  auto pq_a = make_pq(*this);
  auto pq_b = make_pq(other);

  Format res(*suit());

  bool is_pop_a = true;
  bool is_pop_b = true;

  while (!pq_a.empty() and !pq_b.empty()) {
    auto ax_a = pq_a.top();
    if (is_pop_a) pq_a.pop();
    auto ax_b = pq_b.top();
    if (is_pop_b) pq_b.pop();

    if (ax_a < ax_b) {
      res.insert(ax_a);
      pq_b.push(ax_b - ax_a);
      is_pop_a = true;
      is_pop_b = false;
    } else if (ax_a > ax_b) {
      res.insert(ax_b);
      pq_a.push(ax_a - ax_b);
      is_pop_a = false;
      is_pop_b = true;
    } else {
      res.insert(ax_a);
      is_pop_a = true;
      is_pop_b = true;
    }
  }

  return res;
}
}  // namespace rankup

namespace rankup {
void Composition::Starts::insert(int8_t start) {
  m_data.push_back(start);
  m_sorted = false;
}

const int8_t& Composition::Starts::greatest() const {
  if (!m_sorted) {
    std::sort(m_data.begin(), m_data.end());
    m_sorted = true;
  }
  return m_data.back();
}

int8_t Composition::insert(int8_t axle, int8_t start) {
  auto idx = Format::insert(axle);
  m_start[idx].insert(start);
  return idx;
}

bool Composition::defeats(const Composition& other) const {
  if (total_num_cards() != other.total_num_cards()) {
    throw std::runtime_error(
        "Composition::defeats called with mismatched total number of cards!");
  }

  if (suit() == other.suit()) {
    if (is_covered_by(other)) {
      // this defeats `other` by having the greatest of the highest axle >=
      // that in `other`.
      auto idx_ax_highest = get_index_highest_axle();
      auto ax_highest = m_axle[idx_ax_highest];
      auto max_start_ax_highest = m_start[idx_ax_highest].greatest();

      for (int i = 0; i < other.m_axle.size(); ++i) {
        auto ax_o = other.m_axle[i];
        if (ax_o < ax_highest) continue;
        // NOTE to correct if ax_o > ax_highest
        auto max_start_o = other.m_start[i].greatest() + ax_o - ax_highest;
        if (max_start_o > max_start_ax_highest) return false;
      }
      return true;
    } else {
      // other's axle structure doesn't match, so it loses
      return true;
    }
  } else if (suit() == Suit::J) {
    // other isn't composed of lords, so always loses
    return true;
  } else if (other.suit() == Suit::J) {
    // check if other has the same format
    return is_covered_by(other);
  } else {
    // other has folks of a different suit than this, so always loses
    return true;
  }
}
}  // namespace rankup

namespace rankup {
Format RoundRules::get_required_format(const std::vector<Card>& hand) const {
  // TOLDO assert(m_fmt has suit and m_fmt.total_num_cards > 0). This should be
  // guaranteed once RoundRules is created.

  std::vector<Card> cards;
  {
    // extract all cards with the given suit
    // TOLDO can we make it lazy?
    std::copy_if(hand.begin(), hand.end(), std::back_inserter(cards),
                 [suit_given = *m_fmt.suit()](const auto& card) {
                   return card.suit() == suit_given;
                 });
  }
  if (cards.empty()) return {};

  auto enh_cmp = *(m_rules.parse_for_single_suit(cards));

  if (enh_cmp.empty_minor_lord_pairs()) {
    return m_fmt.extract_required_format_from(enh_cmp.cmp.format());
  } else {
    // CLAIM: m_fmt is played by the first player, so it's impossible that m_fmt
    // has mixed axles if it is lord. In this case, one should just use
    // direct_append_extra
    return m_fmt.extract_required_format_from(
        enh_cmp.direct_append_extra().format());
  }
}

bool RoundRules::update_if_defeated_by(const std::vector<Card>& cards) {
  if (cards.size() != m_winning_cmp.format().total_num_cards()) {
    throw std::runtime_error(
        "RoundRules::update_if_defeated_by called with mismatched total number "
        "of cards!");
  }

  auto enh_cmp_opt = m_rules.parse_for_single_suit(cards);

  if (!enh_cmp_opt) {
    return false;
  }

  auto defeated_by = [this](auto&& cmp) {
    if (m_winning_cmp.defeats(cmp)) return false;
    // update the current composition to be the winner
    m_winning_cmp = std::move(cmp);
    return true;
  };

  if (enh_cmp_opt->empty_minor_lord_pairs()) {
    return defeated_by(std::move(enh_cmp_opt->cmp));
  } else {
    // out of the two possible interpretations, at most one matches the format
    // of m_cmp and hence stands a (very high) chance to win.
    if (defeated_by(enh_cmp_opt->direct_append_extra()))
      return true;
    else
      return defeated_by(enh_cmp_opt->split_merge_extra());
  }
}
}  // namespace rankup

namespace rankup {
std::string Rules::check_valid_as_first_cards(
    const std::vector<Card>& hand, const std::vector<bool>& selected) const {
  // TODO implement
}

RoundRules Rules::start_round_with(const std::vector<Card>& cards) const {
  auto enh_cmp_opt = parse_for_single_suit(cards);

  if (!enh_cmp_opt) {
    throw std::runtime_error(
        "Rules::start_round_with called with cards of non-uniform suit!");
  }

  // NOTE that when starting a round, minor lord complication is resolved only
  // using the direct_append_extra() method.
  Composition cmp = enh_cmp_opt->empty_minor_lord_pairs()
                        ? enh_cmp_opt->cmp
                        : enh_cmp_opt->direct_append_extra();

  return RoundRules(*this, std::move(cmp));
}
}  // namespace rankup

namespace rankup {

Composition Rules::EnhancedComposition::direct_append_extra() const {
  // TODO implement
}

Composition Rules::EnhancedComposition::split_merge_extra() const {
  // TODO implement
}

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
    const std::vector<Card>& cards, const Rules::RulesImpl& impl) {
  bool single_suit = true;
  Suit suit;
  std::vector<Value> values;
  values.reserve(cards.size());

  auto lorded_suit = [&impl](const Card& card) {
    // Here we use Suit::J to represent all lords
    return impl.is_lord(card) ? Suit::J : card.suit();
  };

  suit = lorded_suit(cards[0]);
  values.push_back(impl.evaluate(cards[0]));
  for (auto i = 1u; i < cards.size(); ++i) {
    if (lorded_suit(cards[i]) != suit) {
      single_suit = false;
      break;
    }
    values.push_back(impl.evaluate(cards[i]));
  }

  if (single_suit) {
    std::sort(values.begin(), values.end());
  }

  return {single_suit, suit, values};
}

struct Component {
  int8_t axle;
  int8_t start;
};

/**
   @param values, values are sorted, free of complications due to minor lords.
 */
std::vector<Component> parse_to_components(const std::vector<Value>& values) {
  std::vector<Component> res;
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

  // append a sentinel to `values` to make sure the last element in
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

std::optional<Rules::EnhancedComposition> Rules::parse_for_single_suit(
    const std::vector<Card>& cards) const {
  // TOLDO is this necessary
  if (cards.size() == 0) {
    throw std::runtime_error("Parsing empty cards is forbidden!");
  }
  std::optional<Rules::EnhancedComposition> enh_cmp;

  auto [is_single_suit, suit, values] =
      parse_impl::construct_sorted_values(cards, *m_impl);
  if (!is_single_suit) return enh_cmp;

  auto extra_minor_lord_pairs = m_impl->adjust_for_minor_lords(values);

  Composition cmp(suit);
  for (const auto& [axle, start] : parse_impl::parse_to_components(values)) {
    cmp.insert(axle, start);
  }

  auto minor_lord_comps =
      parse_impl::parse_to_components(extra_minor_lord_pairs);
  std::vector<int8_t> ml_pairs;
  ml_pairs.reserve(minor_lord_comps.size());
  for (const auto& [axle, start] : minor_lord_comps) {
    assert(axle == 1);
    ml_pairs.push_back(start);
  }

  enh_cmp.emplace(std::move(cmp), std::move(ml_pairs));

  return enh_cmp;
}
}  // namespace rankup