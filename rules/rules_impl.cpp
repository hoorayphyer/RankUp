#include "rules_impl.hpp"

#include <algorithm>

namespace rankup {

std::vector<Value> Rules::RulesImpl::adjust_for_minor_lords(
    std::vector<Value>& sorted_values, const int8_t minor_lord_val,
    bool allow_adjacent_pair_to_the_left_of_minor_lords) {
  std::vector<Value> res;

  int idx_minors_begin = -1;
  int idx_minors_end = -1;
  // count is indexed by suit
  std::array<int8_t, 4> count = {0, 0, 0, 0};
  for (auto i = 0u; i < sorted_values.size(); ++i) {
    if (sorted_values[i].is_minor_lord()) {
      if (idx_minors_begin == -1) idx_minors_begin = i;
      idx_minors_end = i + 1;
      auto suit = sorted_values[i].minor();
      ++count[suit];
    }
  }

  auto itr_first_pair =
      std::find(count.begin(), count.end(), static_cast<int8_t>(2));
  // no action when there is no pair of minor lords
  if (itr_first_pair == count.end()) return res;

  int suit_first_pair = std::distance(count.begin(), itr_first_pair);

  assert(0 <= idx_minors_begin and idx_minors_begin < idx_minors_end);

  // check if there is a pair of values one less of minor lords
  bool has_pair_of_one_less =
      (allow_adjacent_pair_to_the_left_of_minor_lords and
       idx_minors_begin > 1 and
       sorted_values[idx_minors_begin - 1].major() == minor_lord_val - 1 and
       sorted_values[idx_minors_begin - 2].major() == minor_lord_val - 1);
  // check if there is a pair of major lords
  bool has_pair_of_major_lords =
      (idx_minors_end < sorted_values.size() - 1 and
       sorted_values[idx_minors_end].major() == minor_lord_val + 1 and
       sorted_values[idx_minors_end + 1].major() == minor_lord_val + 1);

  if (has_pair_of_major_lords) {
    auto ritr = std::find(count.rbegin(), count.rend(), static_cast<int8_t>(2));
    assert(ritr != count.rend());
    // above assertion guarantees the following is well behaved.
    int suit_last_pair = 3 - std::distance(count.rbegin(), ritr);
    // find the idx of the first card in the pair
    int idx_begin_last_pair = idx_minors_begin;
    for (int i = 0; i < suit_last_pair - 1; ++i)
      idx_begin_last_pair += count[i];
    if (has_pair_of_one_less) {
      // In this most complicated case, all minor lords except the last pair
      // must come out of the array. Singles can be reinserted back to the end
      // of the array. Extra pairs need to be sotred separately.

      // extract all then erase all to avoid index invalidation
      auto erase_begin1 = sorted_values.begin() + idx_minors_begin;
      auto erase_end1 = sorted_values.begin() + idx_begin_last_pair;
      auto erase_begin2 = sorted_values.begin() + idx_begin_last_pair + 2;
      auto erase_end2 = sorted_values.begin() + idx_minors_end;
      std::vector<Value> extracted(erase_begin1, erase_end1);
      extracted.insert(extracted.end(), erase_begin2, erase_end2);
      sorted_values.erase(erase_begin1, erase_end1);
      sorted_values.erase(erase_begin2, erase_end2);
      auto itr = extracted.begin();
      for (int8_t suit = 0; suit < 4; ++suit) {
        if (suit == suit_last_pair or count[suit] == 0) continue;
        if (count[suit] == 1) {
          sorted_values.push_back(*itr++);
        } else {
          res.push_back(*itr++);
          res.push_back(*itr++);
        }
      }
    } else {
      // need to move any singles between the last pair of minor lords and
      // the pair of major lords to before the last pair of minor lords.
      std::swap(sorted_values[idx_minors_end - 1],
                sorted_values[idx_begin_last_pair + 1]);
      std::swap(sorted_values[idx_minors_end - 2],
                sorted_values[idx_begin_last_pair]);
    }
  } else if (has_pair_of_one_less) {
    // need to move any singles between the first pair of minor lords and the
    // pair of one less to after the first pair of minor lords

    // find the idx of the first card in the pair
    int idx_begin_first_pair = idx_minors_begin;
    for (int i = 0; i < suit_first_pair - 1; ++i)
      idx_begin_first_pair += count[i];
    std::swap(sorted_values[idx_minors_begin + 1],
              sorted_values[idx_begin_first_pair]);
    std::swap(sorted_values[idx_minors_begin + 2],
              sorted_values[idx_begin_first_pair + 1]);
  }

  return res;
}

Value RulesLordful::evaluate(const Card& card) const {
  static_assert(static_cast<int8_t>(Rank::_2) == 0);
  int8_t rank = static_cast<int8_t>(card.rank());
  int8_t lord_rank = static_cast<int8_t>(m_lord.rank());

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
  return Rules::RulesImpl::adjust_for_minor_lords(sorted_values, MINOR_LORD_VAL,
                                                  true);
}

Value RulesLordlessOverthrown::evaluate(const Card& card) const {
  static_assert(static_cast<int8_t>(Rank::_2) == 0);
  int8_t rank = static_cast<int8_t>(card.rank());
  int8_t lord_rank = static_cast<int8_t>(m_lord.rank());

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
  return Rules::RulesImpl::adjust_for_minor_lords(sorted_values, MINOR_LORD_VAL,
                                                  false);
}

Value RulesLordlessRegular::evaluate(const Card& card) const {
  static_assert(static_cast<int8_t>(Rank::_2) == 0);
  int8_t rank = static_cast<int8_t>(card.rank());

  return Value(rank);  // also works for wW
}

bool RulesLordlessRegular::is_lord(const Card& card) const {
  return card.suit() == Suit::J;
}
}  // namespace rankup