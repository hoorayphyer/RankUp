#include <catch2/catch.hpp>
#include <cstdint>
#include <variant>

#include "common/card.hpp"
#include "rules.hpp"
#include "rules_impl.hpp"

using namespace rankup;

SCENARIO("test Value class", "[rules]") {
  const int8_t val_raw = 8;

  WHEN("card is not a minor lord") {
    Value value(val_raw);
    CHECK_FALSE(value.is_minor_lord());
    CHECK((int)value.major() == val_raw);
    CHECK((int)value.full() == (val_raw << 3));
  }

  WHEN("card is a minor lord") {
    Suit suit = Suit::H;
    Value value(val_raw, true, suit);
    CHECK(value.is_minor_lord());
    CHECK((int)value.major() == val_raw);
    const int suit_int = static_cast<int>(suit);
    CHECK((int)value.minor() == suit_int);
    CHECK((int)value.full() == (val_raw << 3) + (suit_int << 1) + 1);
  }
}

SCENARIO("Lordful Rules", "[rules]") {
  const Suit suit_lord = Suit::S;
  const Card lord(suit_lord, Rank::_8);
  auto rules = RulesLordful(lord);

  SECTION("test `evaluate` and `is_lord`") {
    WHEN("card comes before the lord card") {
      Card card(suit_lord, Rank::_4);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == static_cast<int>(Rank::_4));
    }

    WHEN("card comes after the lord card") {
      Card card(suit_lord, Rank::_Q);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == static_cast<int>(Rank::_Q) - 1);
    }

    WHEN("card is a minor lord") {
      Suit suit = Suit::H;
      Card card(suit, Rank::_8);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK(value.is_minor_lord());
      CHECK((int)value.major() == 12);
      CHECK((int)value.minor() == static_cast<int>(suit));
    }

    WHEN("card is a major lord") {
      Card card(suit_lord, Rank::_8);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == 13);
    }

    WHEN("card is a Joker") {
      Card card(Suit::J, Rank::_w);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == 14);
    }

    WHEN("card is not a lord") {
      CHECK_FALSE(rules.is_lord({Suit::D, Rank::_7}));
      CHECK_FALSE(rules.is_lord({Suit::C, Rank::_10}));
      CHECK_FALSE(rules.is_lord({Suit::H, Rank::_A}));
    }
  }

  SECTION("test `adjust_for_minor_lords`") {
    WHEN("cards don't involve minor lords") {
      std::vector<Value> values = {{9}, {9}, {11}, {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN("cards involve single minor lords") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN("cards involve pair minor lords but no left or right pairs") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN(
        "cards involve pair minor lords and a pair of one less but no pair of "
        "major lords") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {12, true, Suit::H},
                                   {13}};
      auto extra = rules.adjust_for_minor_lords(values);
      THEN(
          "singles in between must be shifted to after the first pair of minor "
          "lords") {
        const std::vector<Value> values_exp = {{9},
                                               {9},
                                               {11},
                                               {11},
                                               {12, true, Suit::C},
                                               {12, true, Suit::C},
                                               {12, true, Suit::D},
                                               {12, true, Suit::H},
                                               {12, true, Suit::H},
                                               {13}};
        REQUIRE(values == values_exp);
        REQUIRE(extra == std::vector<Value>{});
      }
    }

    WHEN(
        "cards involve pair minor lords and a pair of major lords but no pair "
        "of one less") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13},
                                   {13}};
      auto extra = rules.adjust_for_minor_lords(values);
      THEN(
          "singles in between must be shifted to before the last pair of minor "
          "lords") {
        const std::vector<Value> values_exp = {{9},
                                               {9},
                                               {11},
                                               {12, true, Suit::D},
                                               {12, true, Suit::D},
                                               {12, true, Suit::H},
                                               {12, true, Suit::C},
                                               {12, true, Suit::C},
                                               {13},
                                               {13}};
        REQUIRE(values == values_exp);
        REQUIRE(extra == std::vector<Value>{});
      }
    }

    WHEN(
        "cards involve pair minor lords as well as a pair of major lords and a "
        "pairof one less") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13},
                                   {13},
                                   {14}};
      auto extra = rules.adjust_for_minor_lords(values);
      THEN(
          "all pairs except the last one are stored as extra, all singles are "
          "moved to the end") {
        const std::vector<Value> values_exp = {{9},
                                               {9},
                                               {11},
                                               {11},
                                               {12, true, Suit::C},
                                               {12, true, Suit::C},
                                               {13},
                                               {13},
                                               {14},
                                               {12, true, Suit::H}};
        const std::vector<Value> extra_exp = {{12, true, Suit::D},
                                              {12, true, Suit::D}};
        REQUIRE(values == values_exp);
        REQUIRE(extra == extra_exp);
      }
    }
  }
}

SCENARIO("Overthrown Lordless Rules", "[rules]") {
  const Suit suit_lord = Suit::J;
  const Card lord(suit_lord, Rank::_8);
  auto rules = RulesLordlessOverthrown(lord);

  SECTION("test `evaluate` and `is_lord`") {
    WHEN("card comes before the lord card") {
      Card card(Suit::S, Rank::_4);
      auto value = rules.evaluate(card);
      REQUIRE_FALSE(rules.is_lord(card));
      CHECK((int)value.major() == static_cast<int>(Rank::_4));
    }

    WHEN("card comes after the lord card") {
      Card card(Suit::S, Rank::_Q);
      auto value = rules.evaluate(card);
      REQUIRE_FALSE(rules.is_lord(card));
      CHECK((int)value.major() == static_cast<int>(Rank::_Q) - 1);
    }

    WHEN("There are no major lords") {
      for (auto suit :
           std::array<Suit, 4>{Suit::D, Suit::C, Suit::H, Suit::S}) {
        Card card(suit, Rank::_8);
        auto value = rules.evaluate(card);
        REQUIRE(rules.is_lord(card));
        CHECK(value.is_minor_lord());
        CHECK((int)value.major() == 12);
        CHECK((int)value.minor() == static_cast<int>(suit));
      }
    }

    WHEN("card is a Joker") {
      Card card(Suit::J, Rank::_w);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == 13);
    }

    WHEN("card is not a lord") {
      CHECK_FALSE(rules.is_lord({Suit::D, Rank::_7}));
      CHECK_FALSE(rules.is_lord({Suit::C, Rank::_10}));
      CHECK_FALSE(rules.is_lord({Suit::H, Rank::_A}));
      CHECK_FALSE(rules.is_lord({Suit::S, Rank::_5}));
    }
  }

  SECTION("test `adjust_for_minor_lords`") {
    WHEN("cards don't involve minor lords") {
      std::vector<Value> values = {{9}, {9}, {11}, {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN("cards involve single minor lords") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN("cards involve pair minor lords but no pair of major lords") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {12, true, Suit::H},
                                   {13}};
      const auto values_orig = values;
      auto extra = rules.adjust_for_minor_lords(values);
      REQUIRE(values == values_orig);
      REQUIRE(extra == std::vector<Value>{});
    }

    WHEN("cards involve pair minor lords and a pair of major lords") {
      std::vector<Value> values = {{9},
                                   {9},
                                   {11},
                                   {12, true, Suit::D},
                                   {12, true, Suit::D},
                                   {12, true, Suit::C},
                                   {12, true, Suit::C},
                                   {12, true, Suit::H},
                                   {13},
                                   {13}};
      auto extra = rules.adjust_for_minor_lords(values);
      THEN(
          "singles in between must be shifted to before the last pair of minor "
          "lords") {
        const std::vector<Value> values_exp = {{9},
                                               {9},
                                               {11},
                                               {12, true, Suit::D},
                                               {12, true, Suit::D},
                                               {12, true, Suit::H},
                                               {12, true, Suit::C},
                                               {12, true, Suit::C},
                                               {13},
                                               {13}};
        REQUIRE(values == values_exp);
        REQUIRE(extra == std::vector<Value>{});
      }
    }
  }
}

SCENARIO("Regular Lordless Rules", "[rules]") {
  const Suit suit_lord = Suit::J;
  const Card lord(suit_lord, Rank::_w);
  auto rules = RulesLordlessRegular(lord);

  SECTION("test `evaluate` and `is_lord`") {
    WHEN("all cards except Jokers are folks") {
      for (auto suit :
           std::array<Suit, 4>{Suit::D, Suit::C, Suit::H, Suit::S}) {
        for (int8_t i = 0; i < 13; ++i) {
          Card card(suit, static_cast<Rank>(i));
          auto value = rules.evaluate(card);
          REQUIRE_FALSE(rules.is_lord(card));
          CHECK((int)value.major() == i);
        }
      }
    }

    WHEN("card is a Joker") {
      Card card(Suit::J, Rank::_w);
      auto value = rules.evaluate(card);
      REQUIRE(rules.is_lord(card));
      CHECK_FALSE(value.is_minor_lord());
      CHECK((int)value.major() == 13);
    }
  }

  SECTION("test `adjust_for_minor_lords`, which is no-op!") {
    std::vector<Value> values = {{9},  {9},  {11}, {11}, {12},
                                 {12}, {13}, {13}, {14}};
    const auto values_orig = values;
    auto extra = rules.adjust_for_minor_lords(values);
    REQUIRE(values == values_orig);
    REQUIRE(extra == std::vector<Value>{});
  }
}

namespace rankup {
class TestRules {
 public:
  TestRules(Card lord_card) : m_rules(std::move(lord_card)) {}

  // functions to be tested
  auto parse(const std::vector<Card>& cards) const {
    return m_rules.parse_for_single_suit(cards);
  }

  // helper functions
  auto evaluate(const Card& card) const {
    return m_rules.m_impl->evaluate(card);
  }

  const auto& lord_card() const { return m_rules.m_lord_card; }

 private:
  Rules m_rules;
};
}  // namespace rankup

std::vector<Card> gen_for_suit(Suit suit, const std::vector<Rank>& ranks) {
  std::vector<Card> res;
  for (auto rank : ranks) {
    res.emplace_back(suit, rank);
  }
  return res;
}

namespace {
// copied from A Tour of C++, page 176
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

// add a deduction guide
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}  // namespace

namespace {
struct VariCard {
 public:
  VariCard(Rank rank) : m_data(rank) {}
  VariCard(Card card) : m_data(std::move(card)) {}
  VariCard(Suit suit, Rank rank) : m_data(Card(suit, rank)) {}
  const auto& data() const { return m_data; }

 private:
  std::variant<Rank, Card> m_data;
};

class CompositionGenerator {
 public:
  CompositionGenerator(const TestRules& testrules) : m_rules(testrules) {}

  Composition operator()(
      Suit suit,
      const std::vector<std::pair<int8_t, VariCard>>& vec_of_axle_card_pairs) {
    Composition res(suit == m_rules.lord_card().suit() ? Suit::J : suit);

    for (const auto& [axle, vari_card] : vec_of_axle_card_pairs) {
      auto val = std::visit(
          overloaded{[&](Rank rank) {
                       return m_rules.evaluate({suit, rank});
                     },
                     [&](const Card& card) { return m_rules.evaluate(card); }},
          vari_card.data());
      // store major() in Component
      res.insert(axle, val.major());
    }
    return res;
  }

 private:
  const TestRules& m_rules;
};
}  // namespace

// Lordful should be enough to cover all cases
SCENARIO("Rules::parse when Lordful", "[rules]") {
  const Suit suit_lord = Suit::S;
  const Card lord(suit_lord, Rank::_8);
  TestRules rules(lord);
  CompositionGenerator gen_cmp(rules);

  SECTION("Test multiple suits") {
    const std::vector<Card> cards = {
        {Suit::D, Rank::_4}, {Suit::D, Rank::_4}, {Suit::S, Rank::_5}};
    auto enh_cmp = rules.parse(cards);
    REQUIRE_FALSE(enh_cmp);
  }

  SECTION("Test folk suits") {
    const Suit suit = Suit::D;
    const std::vector<Rank> ranks = {Rank::_3, Rank::_4, Rank::_4, Rank::_5,
                                     Rank::_6, Rank::_6, Rank::_7, Rank::_7,
                                     Rank::_9, Rank::_9, Rank::_A};
    const std::vector<Card> cards = gen_for_suit(suit, ranks);
    auto enh_cmp = rules.parse(cards);

    REQUIRE(enh_cmp);
    CHECK(enh_cmp->empty_minor_lord_pairs());
    CHECK(enh_cmp->cmp.suit() == suit);
    CHECK(enh_cmp->cmp.total_num_cards() == ranks.size());
    const auto cmp_exp = gen_cmp(suit, {{0, Rank::_3},
                                        {1, Rank::_4},
                                        {0, Rank::_5},
                                        {3, Rank::_6},
                                        {0, Rank::_A}});
    CHECK(enh_cmp->cmp == cmp_exp);
  }

  SECTION("Test minor lord complication") {
    const Suit suit = suit_lord;
    const std::vector<Card> cards = {
        {suit, Rank::_A},    {suit, Rank::_A},    {Suit::C, Rank::_8},
        {Suit::D, Rank::_8}, {Suit::D, Rank::_8}, {Suit::H, Rank::_8},
        {Suit::H, Rank::_8}, {suit, Rank::_8},    {suit, Rank::_8}};
    auto enh_cmp = rules.parse(cards);

    REQUIRE(enh_cmp);
    THEN("pattern uses Suit::J to denote lord cards") {
      CHECK(enh_cmp->cmp.suit() == Suit::J);
    }
    CHECK_FALSE(enh_cmp->empty_minor_lord_pairs());
    CHECK(enh_cmp->cmp.total_num_cards() +
              2 * enh_cmp->extra_ml_pair_start.size() ==
          cards.size());
    const auto cmp_exp =
        gen_cmp(suit, {{3, Rank::_A}, {0, {Suit::C, Rank::_8}}});
    CHECK(enh_cmp->cmp == cmp_exp);
    CHECK(enh_cmp->extra_ml_pair_start ==
          std::vector<int8_t>{rules.evaluate({Suit::D, Rank::_8}).major()});
  }
}

SCENARIO("EnhancedComposition::direct_append_extra and split_merge_extra",
         "[rules]") {}

SCENARIO("Rules::start_round_with", "[rules]") {}

SCENARIO("RoundRules::get_required_format", "[rules]") {}

SCENARIO("RoundRules::update_if_defeated_by", "[rules]") {}

SCENARIO("Composition::defeats", "[rules]") {
  const Suit suit_lord = Suit::S;
  const Card lord(suit_lord, Rank::_8);
  TestRules rules(lord);
  CompositionGenerator gen_cmp(rules);

  SECTION("folk composition") {
    const Suit suit_folk = Suit::C;
    SECTION("pure axle") {
      for (auto axle : std::array{0, 1, 2, 3}) {
        CAPTURE(axle);
        const auto cmp = gen_cmp(suit_folk, {{axle, Rank::_3}});

        SECTION("1st occurrence defeats 2nd occurrence") {
          if (axle == 0) {
            CHECK(cmp.defeats(cmp));
          }
        }
        CHECK(cmp.defeats(gen_cmp(suit_folk, {{axle, Rank::_2}})));
        CHECK_FALSE(cmp.defeats(gen_cmp(suit_folk, {{axle, Rank::_5}})));
        CHECK(cmp.defeats(gen_cmp(Suit::D, {{axle, Rank::_A}})));
        CHECK_FALSE(cmp.defeats(gen_cmp(suit_lord, {{axle, Rank::_2}})));
      }
    }

    SECTION("mixed axles") {
      const auto cmp =
          gen_cmp(suit_folk, {{1, Rank::_10}, {0, Rank::_5}, {0, Rank::_6}});

      THEN("it's defeated by an affording lord card set") {
        CHECK_FALSE(cmp.defeats(gen_cmp(
            suit_lord, {{1, Rank::_10}, {0, Rank::_5}, {0, Rank::_6}})));
      }

      // the following tests may not show up in real world.
      THEN("only the pair needs to be higher") {
        CHECK(cmp.defeats(
            gen_cmp(suit_folk, {{1, Rank::_9}, {0, Rank::_6}, {0, Rank::_7}})));
        CHECK_FALSE(cmp.defeats(
            gen_cmp(suit_folk, {{1, Rank::_J}, {0, Rank::_2}, {0, Rank::_3}})));
      }

      THEN("it defeats higher axle cards if the pair is higher") {
        CHECK(cmp.defeats(gen_cmp(suit_folk, {{2, Rank::_7}})));
      }
    }
  }

  SECTION("lord composition") {
    SECTION("pure axle") {
      for (auto axle : std::array{0, 1, 2, 3}) {
        CAPTURE(axle);
        const auto cmp = gen_cmp(suit_lord, {{axle, Rank::_3}});
        CHECK(cmp.defeats(gen_cmp(suit_lord, {{axle, Rank::_2}})));
        CHECK_FALSE(cmp.defeats(gen_cmp(suit_lord, {{axle, Rank::_5}})));
        CHECK(cmp.defeats(gen_cmp(Suit::D, {{axle, Rank::_A}})));
      }
    }

    SECTION("mixed axles") {
      const auto cmp =
          gen_cmp(suit_lord, {{1, Rank::_10}, {0, Rank::_5}, {0, Rank::_6}});

      THEN("only the pair needs to be higher") {
        CHECK(cmp.defeats(
            gen_cmp(suit_lord, {{1, Rank::_9}, {0, Rank::_6}, {0, Rank::_7}})));
        CHECK_FALSE(cmp.defeats(
            gen_cmp(suit_lord, {{1, Rank::_J}, {0, Rank::_2}, {0, Rank::_3}})));
      }

      THEN("it defeats higher axle cards if the pair is higher") {
        CHECK(cmp.defeats(gen_cmp(suit_lord, {{2, Rank::_7}})));
      }
    }
  }

  WHEN(
      "2 pairs are interpreted as having axles [0,0,1], the axle 1 should "
      "correspond to the larger of the two pairs") {
    const auto cmp =
        gen_cmp(suit_lord, {{0, Rank::_4}, {0, Rank::_4}, {1, Rank::_6}});

    CHECK(cmp.defeats(
        gen_cmp(suit_lord, {{0, Rank::_2}, {0, Rank::_3}, {1, Rank::_5}})));
  }
}

Format gen_format(Suit suit, const std::vector<int8_t>& axles) {
  Format res(suit);
  for (auto axle : axles) res.insert(axle);
  return res;
}

SCENARIO("Format::is_covered_by", "[rules]") {
  SECTION("empty format is covered by any formats") {
    const Format format;
    CHECK(format.is_covered_by(Format()));
    CHECK(format.is_covered_by(gen_format(Suit::C, {5})));
    CHECK(format.is_covered_by(gen_format(Suit::C, {3, 2})));
    CHECK(format.is_covered_by(gen_format(Suit::C, {0, 1, 1})));
  }

  SECTION("folk is covered by lord if affordable") {
    const auto format = gen_format(Suit::C, {0, 0, 1, 2});
    CHECK(format.is_covered_by(gen_format(Suit::J, {0, 0, 1, 2})));
    CHECK_FALSE(format.is_covered_by(gen_format(Suit::D, {0, 0, 1, 2})));
    CHECK(format.is_covered_by(gen_format(Suit::J, {1, 1, 2})));
    CHECK_FALSE(format.is_covered_by(gen_format(Suit::J, {0, 0, 1, 1, 1})));
  }

  SECTION("lord is not covered by folk") {
    const auto format = gen_format(Suit::J, {0, 0, 1, 2});
    CHECK_FALSE(format.is_covered_by(gen_format(Suit::C, {0, 0, 1, 2})));
  }

  SECTION("folk covered by folk, lord covered by lord") {
    for (auto suit : std::array{Suit::C, Suit::J}) {
      const auto format = gen_format(suit, {0, 0, 1, 2});
      CHECK(format.is_covered_by(gen_format(suit, {0, 0, 1, 2})));
      CHECK(format.is_covered_by(gen_format(suit, {1, 1, 1, 2})));
      CHECK(format.is_covered_by(gen_format(suit, {2, 2})));
      CHECK_FALSE(format.is_covered_by(gen_format(suit, {0, 0, 1, 1})));
    }
  }
}

SCENARIO("Format::extract_required_format_from", "[rules]") {
  SECTION("when other can afford the format") {
    const auto format = gen_format(Suit::C, {2, 1, 1, 0, 0});
    CHECK(format ==
          format.extract_required_format_from(gen_format(Suit::C, {5})));
    CHECK(format ==
          format.extract_required_format_from(gen_format(Suit::C, {3, 2})));
    CHECK(format ==
          format.extract_required_format_from(gen_format(Suit::C, {3, 1, 1})));
    CHECK(format == format.extract_required_format_from(
                        gen_format(Suit::C, {3, 1, 0, 0})));
    CHECK(format == format.extract_required_format_from(
                        gen_format(Suit::C, {2, 1, 1, 1})));
    CHECK(format == format.extract_required_format_from(
                        gen_format(Suit::C, {2, 1, 1, 0, 0})));
    CHECK_FALSE(format == format.extract_required_format_from(
                              gen_format(Suit::C, {1, 1, 1, 1, 0, 0})));
  }

  SECTION("2-axle demands pairs") {
    const auto format = gen_format(Suit::C, {2, 1, 1, 0, 0});
    const auto other = gen_format(Suit::C, {1, 1, 1, 1, 1, 0, 0});
    const auto req_fmt = gen_format(Suit::C, {1, 1, 1, 1, 0, 0});
    CHECK(req_fmt == format.extract_required_format_from(other));
  }

  SECTION("when other has fewer cards") {
    const auto format = gen_format(Suit::C, {2, 1, 1, 0, 0});
    {
      std::vector<int8_t> axles = {1, 1, 1, 0, 0, 0};
      const auto other = gen_format(Suit::C, axles);
      const auto req_fmt = other;
      CAPTURE(axles);
      CHECK(req_fmt == format.extract_required_format_from(other));
    }

    WHEN(
        "other has higher axles but fewer cards, the resulting format should "
        "follow that of the format") {
      std::vector<int8_t> axles = {3, 1, 0};
      const auto other = gen_format(Suit::C, axles);
      const auto req_fmt = gen_format(Suit::C, {2, 1, 1, 0});
      ;
      CAPTURE(axles);
      CHECK(req_fmt == format.extract_required_format_from(other));
    }
  }

  SECTION("when other has a different suit") {
    const auto format = gen_format(Suit::C, {2, 1, 1, 0, 0});
    const auto other = gen_format(Suit::D, {2, 1, 1, 0, 0});
    CHECK(Format() == format.extract_required_format_from(other));
  }
}