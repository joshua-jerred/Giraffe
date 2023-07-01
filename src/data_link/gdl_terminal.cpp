#include <iostream>

#include <giraffe_data_link.hpp>

int main() {
  std::cout << "Giraffe Data Link" << std::endl;
  gdl::GdlConfig config;
  gdl::GiraffeDataLink gdl{config};
  return 0;
}