#pragma once
#include "definitions.hpp"

namespace rankup {
struct Card {
 private:
  Suit m_s;
  Rank m_r;

 public:
  constexpr Card(Suit suit, Rank rank) : m_s(suit), m_r(rank) {}

  constexpr const Suit& suit() const noexcept { return m_s; }
  constexpr const Rank& rank() const noexcept { return m_r; }

  constexpr bool operator!=(const Card& other) noexcept {
    return (m_s != other.m_s) or (m_r != other.m_r);
  }

  constexpr bool operator==(const Card& other) noexcept {
    return !(*this != other);
  }
};
}  // namespace rankup