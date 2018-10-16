#include <catch2/catch.hpp>
#include "poker.h"
#include <algorithm>

TEST_CASE("Check generating a deck") {
  auto deck = generate_deck();

  SECTION("We used shrink-to-fit so capacity==size") {
    REQUIRE( deck.size() == 54 );
    REQUIRE( deck.capacity() == 54 );
  }

  auto is_in_deck = [&deck]( const Card& card ) {
                      return std::find(deck.begin(),deck.end(),card) != deck.end();
                    };
  SECTION("check Diamonds") {
    REQUIRE( is_in_deck({DIAMONDS, Rank::_2}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_3}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_4}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_5}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_6}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_7}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_8}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_9}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_10}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_J}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_Q}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_K}));
    REQUIRE( is_in_deck({DIAMONDS, Rank::_A}));
  }
  SECTION("check Clubs") {
    REQUIRE( is_in_deck({CLUBS, Rank::_2}));
    REQUIRE( is_in_deck({CLUBS, Rank::_3}));
    REQUIRE( is_in_deck({CLUBS, Rank::_4}));
    REQUIRE( is_in_deck({CLUBS, Rank::_5}));
    REQUIRE( is_in_deck({CLUBS, Rank::_6}));
    REQUIRE( is_in_deck({CLUBS, Rank::_7}));
    REQUIRE( is_in_deck({CLUBS, Rank::_8}));
    REQUIRE( is_in_deck({CLUBS, Rank::_9}));
    REQUIRE( is_in_deck({CLUBS, Rank::_10}));
    REQUIRE( is_in_deck({CLUBS, Rank::_J}));
    REQUIRE( is_in_deck({CLUBS, Rank::_Q}));
    REQUIRE( is_in_deck({CLUBS, Rank::_K}));
    REQUIRE( is_in_deck({CLUBS, Rank::_A}));
  }
  SECTION("check Hearts") {
    REQUIRE( is_in_deck({HEARTS, Rank::_2}));
    REQUIRE( is_in_deck({HEARTS, Rank::_3}));
    REQUIRE( is_in_deck({HEARTS, Rank::_4}));
    REQUIRE( is_in_deck({HEARTS, Rank::_5}));
    REQUIRE( is_in_deck({HEARTS, Rank::_6}));
    REQUIRE( is_in_deck({HEARTS, Rank::_7}));
    REQUIRE( is_in_deck({HEARTS, Rank::_8}));
    REQUIRE( is_in_deck({HEARTS, Rank::_9}));
    REQUIRE( is_in_deck({HEARTS, Rank::_10}));
    REQUIRE( is_in_deck({HEARTS, Rank::_J}));
    REQUIRE( is_in_deck({HEARTS, Rank::_Q}));
    REQUIRE( is_in_deck({HEARTS, Rank::_K}));
    REQUIRE( is_in_deck({HEARTS, Rank::_A}));
  }
  SECTION("check Spades") {
    REQUIRE( is_in_deck({SPADES, Rank::_2}));
    REQUIRE( is_in_deck({SPADES, Rank::_3}));
    REQUIRE( is_in_deck({SPADES, Rank::_4}));
    REQUIRE( is_in_deck({SPADES, Rank::_5}));
    REQUIRE( is_in_deck({SPADES, Rank::_6}));
    REQUIRE( is_in_deck({SPADES, Rank::_7}));
    REQUIRE( is_in_deck({SPADES, Rank::_8}));
    REQUIRE( is_in_deck({SPADES, Rank::_9}));
    REQUIRE( is_in_deck({SPADES, Rank::_10}));
    REQUIRE( is_in_deck({SPADES, Rank::_J}));
    REQUIRE( is_in_deck({SPADES, Rank::_Q}));
    REQUIRE( is_in_deck({SPADES, Rank::_K}));
    REQUIRE( is_in_deck({SPADES, Rank::_A}));
  }
  SECTION("check Jokers") {
    REQUIRE( is_in_deck({JOKER, Rank::_w}));
    REQUIRE( is_in_deck({JOKER, Rank::_W}));
  }
  SECTION("the following cards shouldn't be there") {
    REQUIRE( !is_in_deck({JOKER, Rank::_2}));
    REQUIRE( !is_in_deck({JOKER, Rank::_3}));
    REQUIRE( !is_in_deck({JOKER, Rank::_4}));
    REQUIRE( !is_in_deck({JOKER, Rank::_5}));
    REQUIRE( !is_in_deck({JOKER, Rank::_6}));
    REQUIRE( !is_in_deck({JOKER, Rank::_7}));
    REQUIRE( !is_in_deck({JOKER, Rank::_8}));
    REQUIRE( !is_in_deck({JOKER, Rank::_9}));
    REQUIRE( !is_in_deck({JOKER, Rank::_10}));
    REQUIRE( !is_in_deck({JOKER, Rank::_J}));
    REQUIRE( !is_in_deck({JOKER, Rank::_Q}));
    REQUIRE( !is_in_deck({JOKER, Rank::_K}));
    REQUIRE( !is_in_deck({JOKER, Rank::_A}));

    REQUIRE( !is_in_deck({DIAMONDS, Rank::_w}));
    REQUIRE( !is_in_deck({DIAMONDS, Rank::_W}));
    REQUIRE( !is_in_deck({CLUBS, Rank::_w}));
    REQUIRE( !is_in_deck({CLUBS, Rank::_W}));
    REQUIRE( !is_in_deck({HEARTS, Rank::_w}));
    REQUIRE( !is_in_deck({HEARTS, Rank::_W}));
    REQUIRE( !is_in_deck({SPADES, Rank::_w}));
    REQUIRE( !is_in_deck({SPADES, Rank::_W}));
  }
}
