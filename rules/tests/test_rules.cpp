#include <catch2/catch.hpp>

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