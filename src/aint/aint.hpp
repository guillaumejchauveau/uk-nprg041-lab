#ifndef LAB_AINT_AINT_H_
#define LAB_AINT_AINT_H_
#include <iostream>

typedef uint32_t block_t;
typedef uint64_t dblock_t;

class aint {
  size_t capacity_;
  size_t size_;
  block_t *blocks_;
  static void breakout_double_block(const dblock_t &d, block_t &s1, block_t &s2);
 public:
  aint();
  ~aint();

  aint(block_t u);
  aint(char *str);
  aint(const aint &other);
  aint(aint &&other) noexcept;

  aint &operator=(block_t u);
  aint &operator=(char *str);
  aint &operator=(const aint &other);
  aint &operator=(aint &&other) noexcept;

  bool zero() const noexcept;
  char *to_string() const;

  void swap(aint &other);
  size_t size() const noexcept;
  size_t capacity() const noexcept;
  void resize(size_t n);
  void reserve(size_t n);
  void shrink_to_fit();

  bool operator<(const aint &other) const;
  bool operator>(const aint &other) const;
  bool operator<=(const aint &other) const;
  bool operator>=(const aint &other) const;
  bool operator==(const aint &other) const;
  bool operator!=(const aint &other) const;

  aint &operator+=(const aint &other);
  aint &operator-=(const aint &other);
  aint &operator*=(const aint &other);
  aint &operator/=(const aint &other);
  aint &operator%=(const aint &other);
  aint &operator<<=(size_t offset);
  aint &operator>>=(size_t offset);

  aint &operator+(const aint &other) const;
  aint &operator-(const aint &other) const;
  aint &operator*(const aint &other) const;
  aint &operator/(const aint &other) const;
  aint &operator%(const aint &other) const;
  aint &operator<<(size_t offset) const;
  aint &operator>>(size_t offset) const;

  class const_iterator {
   private:
    block_t *ptr_;
    size_t blocks_to_read_;

   public:
    const_iterator(const const_iterator &other);

    const_iterator(block_t *block, size_t block_count);

    const block_t &operator*();

    bool operator==(const const_iterator &other);

    bool operator!=(const const_iterator &other);

    const_iterator &operator++();
  };
  class const_reverse_iterator {
   private:
    block_t *ptr_;
    size_t blocks_to_read_;

   public:
    const_reverse_iterator(const const_reverse_iterator &other);

    const_reverse_iterator(block_t *block, size_t block_count);

    const block_t &operator*();

    bool operator==(const const_reverse_iterator &other);

    bool operator!=(const const_reverse_iterator &other);

    const_reverse_iterator &operator++();
  };
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  friend std::ostream &operator<<(std::ostream &o, const aint &ai);
  friend std::istream &operator>>(std::istream &o, aint &ai);
};

#endif //LAB_AINT_AINT_H_
