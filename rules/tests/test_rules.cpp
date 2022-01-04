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

SCENARIO("Overthrown Lordless Rules", "[rules]") {
  const Suit suit_lord = Suit::J;
  const Card lord(suit_lord, Rank::_8);
  auto rules = RulesLordlessOverthrown(lord);

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
    for (auto suit : std::array<Suit,4>{Suit::D, Suit::C, Suit::H, Suit::S}) {
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

SCENARIO("Regular Lordless Rules", "[rules]") {
  const Suit suit_lord = Suit::J;
  const Card lord(suit_lord, Rank::_w);
  auto rules = RulesLordlessRegular(lord);

  WHEN("all cards except Jokers are folks") {
    for (auto suit : std::array<Suit, 4>{Suit::D, Suit::C, Suit::H, Suit::S}) {
      for ( int8_t i = 0; i < 13; ++i ) {
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