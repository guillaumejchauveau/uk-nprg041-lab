#ifndef LAB_AINT_AINT_H_
#define LAB_AINT_AINT_H_
#include <iostream>

#define BLOCK_WIDTH 32
#define BLOCK_MAX UINT32_MAX
typedef uint32_t block_t;
typedef uint64_t dblock_t;

class aint {
 protected:
  size_t capacity_;
  size_t size_;
  block_t *blocks_;
  static void breakout_dblock(const dblock_t &du, block_t &u0, block_t &u1);
 public:
  aint();
  ~aint();

  aint(block_t u);
  aint(dblock_t du);
  aint(char *str);
  aint(const aint &other);
  aint(aint &&other) noexcept;

  aint &operator=(block_t u);
  aint &operator=(dblock_t du);
  aint &operator=(char *str);
  aint &operator=(const aint &other);
  aint &operator=(aint &&other) noexcept;

  bool zero() const noexcept;
  char *to_string() const;

  void swap(aint &other) noexcept;
  size_t size() const noexcept;
  size_t capacity() const noexcept;
  void resize(size_t n);
  void reserve(size_t n);
  void shrink_to_fit();
  void refresh_size();

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

  aint operator+(const aint &other) const;
  aint operator-(const aint &other) const;
  aint operator*(const aint &other) const;
  aint operator/(const aint &other) const;
  aint operator%(const aint &other) const;
  aint operator<<(size_t offset) const;
  aint operator>>(size_t offset) const;

  friend std::ostream &operator<<(std::ostream &o, const aint &ai);
  friend std::istream &operator>>(std::istream &o, aint &ai);

#ifdef TEST
  const block_t *get_blocks() {
    return this->blocks_;
  }
#endif
};

#endif //LAB_AINT_AINT_H_
