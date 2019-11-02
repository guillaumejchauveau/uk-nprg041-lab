#ifndef LIST_HEADER
#define LIST_HEADER
#include <iostream>

template<typename T>
struct list {
  T val;
  list *next;
  std::ostream &operator<<(std::ostream &stream) {
    return stream << val << next;
  }
  void push(T newVal) {
    list L;
    L.val = val;
    L.next = this;
    *this = L;
    std::cout << "lol";
  }
  void pop() {
    list *tmp = this;
    *this = *next;
    //delete tmp;
  }
};

#endif
