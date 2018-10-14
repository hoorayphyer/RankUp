#include <iostream>
#include "poker.h"

int main() {

  Suite a = SPADES;

  std::cout << (a == SPADES) << std::endl;

  std::cout << Rank::_w << std::endl;

  std::cout << "Press Enter to quit";
  std::cin.get();
  return 0;
}
