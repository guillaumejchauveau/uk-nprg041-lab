#include <iostream>
#include "list.h"
/*
void freeList (list **L)
{
  while (*L)
    {
      popList (L);
    }
}
*/


int main() {
  list<int> ll{};
  ll.push(1);
  ll.push(2);
  ll.push(6);
  //bubblesort (&ll);
  std::cout << &ll;
  ll.pop();
  std::cout << &ll;
  //freeList (&ll);
  //std::cout << &ll;
  return 0;
}
