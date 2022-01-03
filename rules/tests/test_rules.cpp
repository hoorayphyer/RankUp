#include <catch2/catch.hpp>
#include "rules_impl.hpp"

using namespace rankup;

SCENARIO("Lordful Rules", "[rules]") {
  const Card lord(Suit::S, Rank::_8);
  auto rules = RulesLordful(lord);

  WHEN("card comes before the lord card") {
    Card card(Suit::S, Rank::_4);
    auto value = rules.evaluate(card);
    REQUIRE_FALSE(value.is_minor_lord());
    REQUIRE((int)value.major() == static_cast<int>(Rank::_4));
  }

  WHEN("card comes after the lord card") {
    Card card(Suit::S, Rank::_Q);
    auto value = rules.evaluate(card);
    REQUIRE_FALSE(value.is_minor_lord());
    REQUIRE((int)value.major() == static_cast<int>(Rank::_Q) - 1);
  }
}