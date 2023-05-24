#include <functional>
#include <iostream>
#include <string>
#include <utility>

#include "configuration_structure.hpp"
#include "validation.h"

int main() {
  std::cout << validate("test_str", 1, 20) << std::endl;
  std::cout << validate("", 2, 20) << std::endl;
  std::cout << validate("testasdddddddddddddddddd_str", 1, 20) << std::endl;
  return 0;
}