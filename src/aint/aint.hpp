#ifndef LAB_AINT_AINT_H_
#define LAB_AINT_AINT_H_
#include <iostream>
#include <exception>

#define BLOCK_WIDTH 32
#define BLOCK_MAX UINT32_MAX
typedef uint32_t block_t;
typedef uint64_t dblock_t;

/**
 * Arbitrary-length unsigned integer class. The a-integer is represented with a
 * set of 32 bits unsigned integers called `blocks`.
 *
 * @details
 * The size of the a-integer is the number of blocks used, excluding any
 * "trailing" zero blocks (to compare with binary numbers, the size of `0010`
 * is 2, not 4).
 * The capacity of an a-integer is the number of blocks currently allocated in
 * memory (the capacity of `0010` would be 4).
 *
 * @note
 * Trailing blocks are the last of the a-integer's blocks, but actually contain
 * the most significant value. The blocks can be seen as a little endian
 * representation with a 2^32 base.
 */
class aint {
 protected:
  /**
   * Current number of blocks allocated in memory.
   */
  size_t capacity_;
  /**
   * Current number of blocks representing the aint.
   */
  size_t size_;
  /**
   * Array containing the blocks.
   */
  block_t *blocks_;
  /**
   * Splits a 64 bits unsigned integer (or double-block) into two 32 bits
   * unsigned integers (simple blocks).
   * @param du The double-block to split
   * @param u0 The block that will contain the first 32 bits (bits 0 to 31) of
   * the double-block
   * @param u1 The block that will contain the last 32 bits (bits 32 to 63) of
   * the double-block
   */
  static void breakout_dblock(const dblock_t &du, block_t &u0, block_t &u1);
 public:
  aint();
  ~aint();

  aint(block_t u);
  static aint from_dblock(dblock_t du);
  aint(char *str);
  aint(const aint &other);
  aint(aint &&other) noexcept;

  /**
   * Replaces the blocks of the aint with a single block.
   * @param u The value for the new block
   * @return self
   */
  aint &operator=(block_t u);
  /**
   * Parses a little-endian binary representation of an unsigned integer uses it
   * to set the aint's value.
   * @param str A little-endian binary representation of an unsigned integer
   * @return self
   */
  aint &operator=(char *str);
  aint &operator=(const aint &other);
  aint &operator=(aint &&other) noexcept;

  /**
   * Checks if the aint equals zero.
   */
  bool zero() const noexcept;
  /**
   * @return A little-endian binary representation of the aint
   */
  char *to_string() const;

  void swap(aint &other) noexcept;
  /**
   * @return Current number of blocks representing the aint.
   */
  size_t size() const noexcept;
  /**
   * @return Current number of blocks allocated in memory.
   */
  size_t capacity() const noexcept;
  /**
   * Modifies the capacity of the aint with memory reallocation. If resizing to
   * a lower capacity, blocks may be lost; the size of the aint is updated
   * accordingly.
   * @param n The new capacity
   * @throws std::bad_alloc Thrown if an error occurs during reallocation
   */
  void resize(size_t n);
  /**
   * Ensures a minimum capacity for the aint.
   * Returns if the target capacity is lower than the actual capacity.
   * @param n The target capacity
   * @see aint::resize(size_t)
   */
  void reserve(size_t n);
  /**
   * Resizes the aint to reduce the capacity to the actual size of the aint.
   * @see aint::resize(size_t)
   */
  void shrink_to_fit();
  /**
   * Decreases the current size of to the first non-zero "trailing" block.
   */
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
  /**
   * FOR TESTING PURPOSES.
   * @return The array of blocks
   */
  const block_t *get_blocks() {
    return this->blocks_;
  }
#endif
};

#endif //LAB_AINT_AINT_H_
