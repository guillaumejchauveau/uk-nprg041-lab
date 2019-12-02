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
  aint a = const_cast<char *>("11");
  aint b = const_cast<char *>("1");
  aint c = a + b;
  std::cout << c << std::endl;
}
