#include "aint.hpp"
#include <algorithm>
#include <exception>
#include <cstring>
#include <cmath>

void aint::breakout_dblock(const dblock_t& du, block_t& u0, block_t& u1) {
  u0 = static_cast<uint32_t> (du);
  u1 = static_cast<uint32_t> (du >> BLOCK_WIDTH);
}

aint::aint() {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
}

aint::aint(block_t u) {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(u);
}

aint aint::from_dblock(const dblock_t du) {
  if (du == 0) {
    return aint();
  }
  aint a;
  a.resize(2);
  block_t u0, u1;
  aint::breakout_dblock(du, u0, u1);
  a.blocks_[0] = u0;
  a.blocks_[1] = u1;
  a.size_ = 2;
  a.refresh_size();
  return a;
}

aint::aint(char* str) {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(str);
}

aint::aint(const aint& other) {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(other);
}

aint::aint(aint&& other) noexcept {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(std::move(other));
}

aint& aint::operator=(block_t u) {
  if (u == 0) {
    this->resize(0);
    return *this;
  }
  this->resize(1);
  this->blocks_[0] = u;
  this->size_ = 1;
  return *this;
}

aint& aint::operator=(char* str) {
  // Number of blocks that will be required to fit the string.
  size_t str_block_count = (std::strlen(str) + BLOCK_WIDTH - 1) / BLOCK_WIDTH;
  if (str_block_count == 0) {
    this->resize(0);
    return *this;
  }
  this->resize(str_block_count);
  this->size_ = this->capacity();
  // Calculates the value of each blocks.
  for (size_t block_index = 0; block_index < this->size(); block_index++) {
    block_t block = 0;
    for (int bit_index = 0; bit_index < BLOCK_WIDTH; bit_index++) {
      char c = str[block_index * BLOCK_WIDTH + bit_index];
      if (c == '1') {
        block += std::pow(2, bit_index);
      } else if (c == '\0') {
        break;
      } else if (c != '0') {
        this->resize(0);
        throw std::invalid_argument("str");
      }
    }
    // Saves the new block value.
    this->blocks_[block_index] = block;
  }
  this->refresh_size();
  return *this;
}

aint& aint::operator=(const aint& other) {
  if (this == &other) {
    return *this;
  }
  this->resize(other.capacity());
  for (size_t i = 0; i < other.size(); i++) {
    this->blocks_[i] = other.blocks_[i];
  }
  this->size_ = other.size();
  this->refresh_size();
  return *this;
}

aint& aint::operator=(aint&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  free(this->blocks_);
  this->capacity_ = other.capacity_;
  this->size_ = other.size_;
  this->blocks_ = other.blocks_;
  other.capacity_ = 0;
  other.size_ = 0;
  other.blocks_ = nullptr;
  this->refresh_size();
  return *this;
}

aint::~aint() {
  free(this->blocks_);
}

size_t aint::capacity() const noexcept {
  return this->capacity_;
}

size_t aint::size() const noexcept {
  return this->size_;
}

void aint::resize(size_t n) {
  if (n == this->capacity()) {
    return;
  }
  // Sets the aint to zero if the new size is zero.
  if (n == 0) {
    free(this->blocks_);
    this->blocks_ = nullptr;
    this->capacity_ = 0;
    this->size_ = 0;
    return;
  }
  auto new_blocks = static_cast<block_t*>(realloc(this->blocks_, n * sizeof(block_t)));
  if (new_blocks == nullptr) {
    throw std::bad_alloc();
  }
  this->blocks_ = new_blocks;
  this->capacity_ = n;
  // If the new size is smaller than the previous.
  if (this->size() > this->capacity()) {
    this->size_ = n;
  }
  // Resets trailing blocks to zero.
  for (size_t i = this->size(); i < this->capacity(); i++) {
    this->blocks_[i] = 0;
  }
  this->refresh_size();
}

void aint::reserve(size_t n) {
  if (n > this->capacity()) {
    this->resize(n);
  }
}

void aint::shrink_to_fit() {
  this->refresh_size();
  this->resize(this->size());
}

void aint::refresh_size() {
  size_t real_block_count = this->size();
  for (; real_block_count > 0 && this->blocks_[real_block_count - 1] == 0; real_block_count--) {}
  this->size_ = real_block_count;
}

void aint::swap(aint& other) noexcept {
  if (this == &other) {
    return;
  }
  auto tmp_capacity = this->capacity_;
  auto tmp_block_count = this->size_;
  auto tmp_bocks = this->blocks_;
  this->capacity_ = other.capacity_;
  this->size_ = other.size_;
  this->blocks_ = other.blocks_;
  this->refresh_size();
  other.capacity_ = tmp_capacity;
  other.size_ = tmp_block_count;
  other.blocks_ = tmp_bocks;
  other.refresh_size();
}

bool aint::zero() const noexcept {
  return this->blocks_ == nullptr || this->size() == 0;
}

char* aint::to_string() const {
  if (this->zero()) {
    auto render = static_cast<char*>(calloc(2, sizeof(char)));
    if (render == nullptr) {
      throw std::bad_alloc();
    }
    render[0] = '0';
    render[1] = '\0';
    return render;
  }

  auto render = static_cast<char*>(calloc(BLOCK_WIDTH * this->size() + 1, sizeof(char)));
  if (render == nullptr) {
    throw std::bad_alloc();
  }

  for (size_t block_index = 0; block_index < this->size(); block_index++) {
    block_t block = this->blocks_[block_index];
    for (int bit_index = 0; bit_index < BLOCK_WIDTH; bit_index++) {
      render[block_index * BLOCK_WIDTH + bit_index] = block % 2 ? '1' : '0';
      block /= 2;
      // Places the string end character.
      if (block_index == this->size() - 1 && block == 0) {
        render[block_index * BLOCK_WIDTH + bit_index + 1] = '\0';
        break;
      }
    }
  }
  return render;
}
/**
 * Lower/greater-like operations are done using the most-significant block first
 * on to the least. Indices are shifted by one to prevent underflow.
 */
bool aint::operator<(const aint& other) const {
  if (this->size() < other.size()) {
    return true;
  }
  if (this->size() > other.size()) {
    return false;
  }

  for (size_t i = this->size(); 0 < i; i--) {
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return false;
}

bool aint::operator>(const aint& other) const {
  if (this->size() > other.size()) {
    return true;
  }
  if (this->size() < other.size()) {
    return false;
  }

  for (size_t i = this->size(); 0 < i; i--) {
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return false;
}

bool aint::operator<=(const aint& other) const {
  if (this->size() < other.size()) {
    return true;
  }
  if (this->size() > other.size()) {
    return false;
  }

  for (size_t i = this->size(); 0 < i; i--) {
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return true;
}

bool aint::operator>=(const aint& other) const {
  if (this->size() > other.size()) {
    return true;
  }
  if (this->size() < other.size()) {
    return false;
  }

  for (size_t i = this->size(); 0 < i; i--) {
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return true;
}

bool aint::operator==(const aint& other) const {
  if (this->size() != other.size()) {
    return false;
  }

  for (size_t i = 0; i < this->size(); i++) {
    if (*(this->blocks_ + i) != *(other.blocks_ + i)) {
      return false;
    }
  }
  return true;
}

bool aint::operator!=(const aint& other) const {
  if (this->size() != other.size()) {
    return true;
  }

  for (size_t i = 0; i < this->size(); i++) {
    if (*(this->blocks_ + i) != *(other.blocks_ + i)) {
      return true;
    }
  }
  return false;
}

aint& aint::operator+=(const aint& other) {
  if (this->zero()) {
    *this = other;
    return *this;
  }
  size_t new_size = this->size() + 1;
  if (this->size() < other.size()) {
    new_size = other.size() + 1;
  }
  this->reserve(new_size);

  bool overflow = false;
  for (size_t i = 0; i < new_size; i++) {
    if (this->blocks_[i] == 0) {
      this->blocks_[i] += overflow;
      overflow = false;
    } else {
      this->blocks_[i] += overflow;
      // The block is non-zero and we add either zero or one. An overflow can
      // only result in the block being zero (the block was maximum).
      overflow = this->blocks_[i] == 0;
    }
    // If the other aint still has a non-processed block.
    if (other.size() >= i + 1) {
      this->blocks_[i] += other.blocks_[i];
      if (this->blocks_[i] < other.blocks_[i]) {
        overflow = true;
      }
    }
  }
  this->size_ = new_size;
  this->refresh_size();
  return *this;
}

aint& aint::operator-=(const aint& other) {
  if (other > *this) {
    throw std::invalid_argument("Cannot subtract a higher number");
  }
  bool overflow = false;
  block_t tmp;
  for (size_t i = 0; i < this->size(); i++) {
    if (this->blocks_[i] == BLOCK_MAX) {
      this->blocks_[i] -= overflow;
      overflow = false;
    } else {
      this->blocks_[i] -= overflow;
      // The block is non-maximum and we subtract either zero or one. An
      // overflow can only result in the block being maximum (the block was
      // zero).
      overflow = this->blocks_[i] == BLOCK_MAX;
    }
    // If the other aint still has a non-processed block.
    if (other.size() >= i + 1) {
      tmp = this->blocks_[i];
      this->blocks_[i] -= other.blocks_[i];
      if (this->blocks_[i] > tmp) {
        overflow = true;
      }
    }
  }
  this->refresh_size();
  return *this;
}

aint& aint::operator*=(const aint& other) {
  if (this->zero()) {
    return *this;
  }
  if (other.zero()) {
    *this = 0u;
    return *this;
  }
  aint result, product;
  size_t new_size = this->size() + other.size();
  result.reserve(new_size);

  for (size_t other_block_i = 0; other_block_i < other.size(); other_block_i++) {
    for (size_t this_block_i = 0; this_block_i < this->size(); this_block_i++) {
      product = aint::from_dblock(
        static_cast<dblock_t>(this->blocks_[this_block_i])
          * static_cast<dblock_t>(other.blocks_[other_block_i]));
      product <<= (this_block_i + other_block_i) * BLOCK_WIDTH;
      result += product;
    }
  }
  result.size_ = new_size;
  *this = result;
  this->refresh_size();
  return *this;
}

aint& aint::operator/=(const aint& other) {
  if (other.zero()) {
    throw std::invalid_argument("division by zero");
  }
  if (this->zero()) {
    return *this;
  }
  if (*this < other) {
    *this = 0u;
    return *this;
  }
  size_t bit_count = this->size() * BLOCK_WIDTH;
  if (bit_count < this->size()) {
    throw std::overflow_error("Dividend size too large for bit-shifting");
  }
  aint divisor = other, divisor_pow = 1u;
  divisor <<= bit_count; // Align divisor with dividend.
  divisor_pow <<= bit_count; // Save the power of the divisor for the quotient.
  aint& rest = *this; // Just to keep the code clear without creating a new aint.
  aint quotient;
  while (rest >= other) {
    if (rest >= divisor) {
      rest -= divisor;
      quotient += divisor_pow;
    } else {
      divisor >>= 1;
      divisor_pow >>= 1;
    }
  }
  *this = std::move(quotient);
  this->refresh_size();
  return *this;
}

aint& aint::operator%=(const aint& other) {
  if (other.zero()) {
    throw std::invalid_argument("division by zero");
  }
  if (this->zero()) {
    *this = other;
    return *this;
  }
  if (*this < other) {
    return *this;
  }
  size_t bit_count = this->size() * BLOCK_WIDTH;
  if (bit_count < this->size()) {
    throw std::overflow_error("Dividend size too large for bit-shifting");
  }
  aint divisor = other;
  divisor <<= bit_count; // Align divisor with dividend.
  while (*this >= other) {
    if (*this >= divisor) {
      *this -= divisor;
    } else {
      divisor >>= 1;
    }
  }
  this->refresh_size();
  return *this;
}

aint& aint::operator<<=(size_t offset) {
  if (this->zero() || offset == 0) {
    return *this;
  }
  size_t new_size = this->size() + (offset + BLOCK_WIDTH - 1) / BLOCK_WIDTH;
  this->resize(new_size);

  size_t block_offset = offset / BLOCK_WIDTH;
  block_t new_block_a, new_block_b;
  for (size_t i = this->size(); i > 0; i--) {
    new_block_a = this->blocks_[i - 1] << offset % BLOCK_WIDTH;
    // If the bit shift did not overflow.
    if (offset % BLOCK_WIDTH == 0) {
      new_block_b = 0;
    } else {
      // Computes the overflowing part.
      new_block_b = this->blocks_[i - 1] >> (BLOCK_WIDTH - offset % BLOCK_WIDTH);
    }
    this->blocks_[i - 1] = 0;
    this->blocks_[i - 1 + block_offset] = new_block_a;
    // Ensures the block exists.
    if (new_block_b != 0) {
      this->blocks_[i - 1 + block_offset + 1] += new_block_b;
    }
  }
  this->size_ = new_size;
  this->refresh_size();
  return *this;
}

aint& aint::operator>>=(size_t offset) {
  if (this->zero() || offset == 0) {
    return *this;
  }

  size_t block_offset = offset / BLOCK_WIDTH;
  if (block_offset >= this->size()) {
    *this = 0u;
    return *this;
  }
  block_t new_block_a, new_block_b;
  for (size_t i = 0; i < this->size(); i++) {
    // If the bit shift did not overflow.
    if (offset % BLOCK_WIDTH == 0) {
      new_block_a = 0;
    } else {
      // Computes the overflowing part.
      new_block_a = this->blocks_[i] << (BLOCK_WIDTH - offset % BLOCK_WIDTH);
    }
    new_block_b = this->blocks_[i] >> offset % BLOCK_WIDTH;
    this->blocks_[i] = 0;
    // Ensures the block exists.
    if (i >= block_offset + 1) {
      this->blocks_[i - block_offset - 1] += new_block_a;
    }
    // Ensures the block exists.
    if (i >= block_offset) {
      this->blocks_[i - block_offset] = new_block_b;
    }
  }

  this->size_ -= block_offset;
  this->refresh_size();
  return *this;
}

aint aint::operator+(const aint& other) const {
  aint res = *this;
  res += other;
  return res;
}

aint aint::operator-(const aint& other) const {
  aint res = *this;
  res -= other;
  return res;
}

aint aint::operator*(const aint& other) const {
  aint res = *this;
  res *= other;
  return res;
}

aint aint::operator/(const aint& other) const {
  aint res = *this;
  res /= other;
  return res;
}

aint aint::operator%(const aint& other) const {
  aint res = *this;
  res %= other;
  return res;
}

aint aint::operator<<(size_t offset) const {
  aint res = *this;
  res <<= offset;
  return res;
}

aint aint::operator>>(size_t offset) const {
  aint res = *this;
  res >>= offset;
  return res;
}

std::ostream& operator<<(std::ostream& o, const aint& ai) {
  auto str = ai.to_string();
  o << str;
  free(str);
  return o;
}

std::istream& operator>>(std::istream& i, aint& ai) {
  size_t input_buffer_size = 10;
  size_t input_size = 0;
  auto input_buffer = static_cast<char*>(calloc(input_buffer_size, sizeof(char)));
  if (input_buffer == nullptr) {
    throw std::bad_alloc();
  }
  for (char c = i.get(); !i.eof() && c != '\n'; c = i.get()) {
    if (!isspace(c)) {
      if (input_size + 2 > input_buffer_size) {
        input_buffer_size *= 2;
        input_buffer = static_cast<char*>(realloc(input_buffer, input_buffer_size * sizeof(char)));
        if (input_buffer == nullptr) {
          throw std::bad_alloc();
        }
      }
      input_buffer[input_size] = c;
      input_size++;
    }
  }
  input_buffer[input_size] = '\0';
  try {
    ai = input_buffer;
  }
  catch (std::invalid_argument&) {
    i.setstate(std::ios::failbit);
  }
  free(input_buffer);
  return i;
}
