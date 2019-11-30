#include <iostream>
#include "aint/aint.hpp"

int main() {
  /*SVector<int> v;
  v.push_back(3);
  v[0] = 1;
  std::cout << v[0] << std::endl;
  std::cout << v[1200000000] << std::endl;
  v.size = 5;
  v[2] = 5;
  std::cout << v[2] << std::endl;*/
  aint a = const_cast<char *>("0");
  aint b = std::move(a);

  std::cout << b << std::endl;
}
