#include "SVector.cpp"
#include <iostream>

int main() {
  SVector<int> v;
  v.push_back(3);
  v[0] = 1;
  std::cout << v[0] << std::endl;
  std::cout << v[1200000000] << std::endl;
  v.size = 5;
  v[2] = 5;
  std::cout << v[2] << std::endl;
  return 0;
}
