#pragma once

#include <cstdint>

namespace rankup {

using std::int8_t;

enum class Suit : int8_t { D = 0, C, H, S, J };

// _w is Joker low, and _W is Joker high
enum class Rank : int8_t {
  _2 = 0,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
  _10,
  _J,
  _Q,
  _K,
  _A,
  _w,
  _W
};

}  // namespace rankup