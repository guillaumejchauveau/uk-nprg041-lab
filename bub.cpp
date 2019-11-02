#include "bub.h"

template<typename T>
void swap(T &a, T &b) {
  T c = a;
  a = b;
  b = c;
}

template<typename T>
void bubblesort(T *L) {
  struct list<T> *i, *j;
  for (i = L; i; i = i->next) {
    for (j = L; j; j = j->next) {
      if (j->next && j->val > j->next->val) {
        swap(j->val, j->next->val);
      }
    }
  }
}

