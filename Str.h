//
// Created by guillaumejchauveau on 23/10/2019.
//

#ifndef LAB__STR_H_
#define LAB__STR_H_
#include <cstddef>
#include <iostream>

class Str {
 public:
  char *s;
  Str() {
    s = nullptr;
  }
  Str(const Str &text) {
    s = nullptr;
    assign(text.s);
  }
  ~Str() {
    clear();
  }
  void clear() {
    delete s;
    s = nullptr;
  }
  Str &operator=(const char *text) {
    assign(text);
    return *this;
  }
  Str &operator=(const Str &text) {
    assign(text.s);
    return *this;
  }
  void assign(const char *text) {
    size_t length = 0;
    while (text[length++]);
    s = new char[length + 1]{0};
    for (size_t i = 0; i < length; i++) {
      s[i] = text[i];
    }
  }
  size_t length() const {
    size_t length = 0;
    while (s[length++]);
    return length;
  }
  char &operator[](int index) {
    return s[index];
  }

  const char &operator[](int index) const {
    return s[index];
  }
};

std::ostream &operator<<(std::ostream &stream, const Str &text) {
  return stream << text.s;
}

#endif //LAB__STR_H_
