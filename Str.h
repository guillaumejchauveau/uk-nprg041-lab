#ifndef LAB__STR_H_
#define LAB__STR_H_
#include <cstddef>
#include <iostream>

class Str {
 private:
  char *s_;
 public:
  Str();
  ~Str();

  Str(const Str &str);

  explicit Str(const char *other);

  Str(Str &&other) noexcept;

  Str &operator=(const char *str);

  Str &operator=(const Str &str);

  Str &operator=(Str &&str) noexcept;

  Str operator+(const Str &b);

  char &operator[](size_t index);

  const char &operator[](size_t index) const;

  size_t length() const;

  void clear();

  void assign(const char *text);

  class iterator {
   private:
    char *ptr_;

   public:
    iterator(const iterator &other);

    explicit iterator(char *c);

    char &operator*();

    bool operator==(const iterator &other);

    bool operator!=(const iterator &other);

    iterator &operator++();
  };

  iterator begin() const;

  iterator end() const;
};

std::ostream &operator<<(std::ostream &o, const Str &str);

#endif //LAB__STR_H_
