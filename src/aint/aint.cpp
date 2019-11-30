#include "aint.hpp"
#include <algorithm>
#include <exception>
#include <cstring>
#include <cmath>

/**
 * UTILS
 */

void aint::breakout_double_block(const dblock_t &d, block_t &s1, block_t &s2) {
  s1 = (uint32_t) d;
  s2 = (uint32_t) (d >> 32);
}

/**
 * CONSTRUCTORS/ASSIGNMENTS
 */
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
aint::aint(char *str) {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(str);
}
aint::aint(const aint &other) {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(other);
}
aint::aint(aint &&other) noexcept {
  this->capacity_ = 0;
  this->size_ = 0;
  this->blocks_ = nullptr;
  this->operator=(std::move(other));
}

aint &aint::operator=(block_t u) {
  if (u == 0) {
    this->resize(0);
    return *this;
  }
  this->resize(1);
  *this->blocks_ = u;
  this->size_ = 1;
  return *this;
}
aint &aint::operator=(char *str) {
  size_t str_block_count = (std::strlen(str) + 31) / 32;
  if (str_block_count == 0) {
    this->resize(0);
    return *this;
  }
  this->resize(str_block_count);
  this->size_ = this->capacity();
  for (size_t block_number = 0; block_number < this->size(); block_number++) {
    block_t block = 0;
    for (int b = 0; b < 32; b++) {
      char c = str[block_number * 32 + b];
      if (c == '1') {
        block += std::pow(2, b);
      } else if (c == '\0') {
        break;
      } else if (c != '0') {
        throw std::invalid_argument("str");
      }
    }
    this->blocks_[block_number] = block;
  }
  size_t real_block_count = this->size();
  int a = 1;
  for (; this->blocks_[real_block_count - 1] == 0 && real_block_count > 0; real_block_count--);
  this->resize(real_block_count);
  return *this;
}
aint &aint::operator=(const aint &other) {
  if (this == &other) {
    return *this;
  }
  this->size_ = 0;
  this->resize(other.capacity());
  for (const auto &block : other) {
    this->blocks_[this->size_] = block;
    this->size_++;
  }
  return *this;
}
aint &aint::operator=(aint &&other) noexcept {
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
  return *this;
}

/**
 * DESTRUCTOR
 */

aint::~aint() {
  free(this->blocks_);
}

/**
 * MEMBERS
 */

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
  if (n == 0) {
    free(this->blocks_);
    this->blocks_ = nullptr;
    this->capacity_ = 0;
    this->size_ = 0;
    return;
  }
  this->blocks_ = static_cast<block_t *>(reallocarray(this->blocks_, n, sizeof(block_t)));
  if (this->blocks_ == nullptr) {
    throw std::bad_alloc();
  }
  this->capacity_ = n;
  if (this->size() > this->capacity()) {
    this->size_ = n;
  }
}

void aint::reserve(size_t n) {
  if (n > this->capacity()) {
    this->resize(n);
  }
}
void aint::shrink_to_fit() {
  this->resize(this->size());
}

void aint::swap(aint &other) {
  if (this == &other) {
    return;
  }
  auto tmp_capacity = this->capacity_;
  auto tmp_block_count = this->size_;
  auto tmp_bocks = this->blocks_;
  this->capacity_ = other.capacity_;
  this->size_ = other.size_;
  this->blocks_ = other.blocks_;
  other.capacity_ = tmp_capacity;
  other.size_ = tmp_block_count;
  other.blocks_ = tmp_bocks;
}

bool aint::zero() const noexcept {
  return this->blocks_ == nullptr || this->size() == 0;
}

char *aint::to_string() const {
  if (this->zero()) {
    auto render = static_cast<char *>(calloc(2, sizeof(char)));
    if (render == nullptr) {
      throw std::bad_alloc();
    }
    render[0] = '0';
    render[1] = '\0';
    return render;
  }

  auto render = static_cast<char *>(calloc(32 * this->size() + 1, sizeof(char)));
  if (render == nullptr) {
    throw std::bad_alloc();
  }

  for (size_t block_number = 0; block_number < this->size(); block_number++) {
    block_t block = this->blocks_[block_number];
    for (int b = 0; b < 32; b++) {
      render[block_number * 32 + b] = block % 2 ? '1' : '0';
      block /= 2;
      if (block_number == this->size() - 1 && block == 0) {
        render[block_number * 32 + b + 1] = '\0';
        break;
      }
    }
  }

  return render;
}

/**
 * COMPARATORS
 */

bool aint::operator<(const aint &other) const {
  if (this->size() < other.size()) {
    return true;
  }
  if (this->size() > other.size()) {
    return false;
  }

  for (size_t i = this->size(); i > 0; i--) {
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return false;
}

bool aint::operator>(const aint &other) const {
  if (this->size() > other.size()) {
    return true;
  }
  if (this->size() < other.size()) {
    return false;
  }

  for (size_t i = this->size(); i > 0; i--) {
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return false;
}
bool aint::operator<=(const aint &other) const {
  if (this->size() < other.size()) {
    return true;
  }
  if (this->size() > other.size()) {
    return false;
  }

  for (size_t i = this->size(); i > 0; i--) {
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return true;
}

bool aint::operator>=(const aint &other) const {
  if (this->size() > other.size()) {
    return true;
  }
  if (this->size() < other.size()) {
    return false;
  }

  for (size_t i = this->size(); i > 0; i--) {
    if (*(this->blocks_ + i - 1) > *(other.blocks_ + i - 1)) {
      return true;
    }
    if (*(this->blocks_ + i - 1) < *(other.blocks_ + i - 1)) {
      return false;
    }
  }
  return true;
}

bool aint::operator==(const aint &other) const {
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

bool aint::operator!=(const aint &other) const {
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

/**
 * ACCUMULATIONS
 */
aint &aint::operator+=(const aint &other) {
}
aint &aint::operator-=(const aint &other) {
}
aint &aint::operator*=(const aint &other) {
}
aint &aint::operator/=(const aint &other) {
}
aint &aint::operator%=(const aint &other) {
}
aint &aint::operator<<=(size_t offset) {
}
aint &aint::operator>>=(size_t offset) {
}

/**
 * OPERATIONS
 */

aint &aint::operator+(const aint &other) const {
  auto sum = new aint(*this);
  *sum += other;
  return *sum;
}
aint &aint::operator-(const aint &other) const {
  auto sum = new aint(*this);
  *sum -= other;
  return *sum;
}
aint &aint::operator*(const aint &other) const {
  auto sum = new aint(*this);
  *sum *= other;
  return *sum;
}
aint &aint::operator/(const aint &other) const {
  auto sum = new aint(*this);
  *sum /= other;
  return *sum;
}
aint &aint::operator%(const aint &other) const {
  auto sum = new aint(*this);
  *sum %= other;
  return *sum;
}
aint &aint::operator<<(size_t offset) const {
  auto sum = new aint(*this);
  *sum <<= offset;
  return *sum;
}
aint &aint::operator>>(size_t offset) const {
  auto sum = new aint(*this);
  *sum >>= offset;
  return *sum;
}

/**
 * ITERATOR
 */

aint::const_iterator::const_iterator(
    const const_iterator &other) {
  this->ptr_ = other.ptr_;
  this->blocks_to_read_ = other.blocks_to_read_;
}

aint::const_iterator::const_iterator(block_t *block, size_t
block_count) {
  this->ptr_ = block;
  this->blocks_to_read_ = block_count;
}

const block_t &aint::const_iterator::operator*() {
  return *this->ptr_;
}

bool aint::const_iterator::operator==(const const_iterator &other) {
  return this->ptr_ == other.ptr_;
}

bool aint::const_iterator::operator!=(const const_iterator &other) {
  return this->ptr_ != other.ptr_;
}

aint::const_iterator &aint::const_iterator::operator++() {
  this->blocks_to_read_--;

  if (this->blocks_to_read_ == 0) {
    this->ptr_ = nullptr;
  } else {
    this->ptr_++;
  }
  return *this;
}

aint::const_reverse_iterator::const_reverse_iterator(
    const const_reverse_iterator &other) {
  this->ptr_ = other.ptr_;
  this->blocks_to_read_ = other.blocks_to_read_;
}

aint::const_reverse_iterator::const_reverse_iterator(block_t *block, size_t
block_count) {
  this->ptr_ = block;
  this->blocks_to_read_ = block_count;
}

const block_t &aint::const_reverse_iterator::operator*() {
  return *(this->ptr_ + this->blocks_to_read_ - 1);
}

bool aint::const_reverse_iterator::operator==(const const_reverse_iterator &other) {
  return this->ptr_ == other.ptr_;
}

bool aint::const_reverse_iterator::operator!=(const const_reverse_iterator &other) {
  return this->ptr_ != other.ptr_;
}

aint::const_reverse_iterator &aint::const_reverse_iterator::operator++() {
  this->blocks_to_read_--;
  if (this->blocks_to_read_ == 0) {
    this->ptr_ = nullptr;
  }
  return *this;
}

aint::const_iterator aint::cbegin() const {
  return const_iterator(this->blocks_, this->size());
}

aint::const_iterator aint::cend() const {
  return const_iterator(nullptr, 0);
}

aint::const_iterator aint::begin() const {
  return this->cbegin();
}

aint::const_iterator aint::end() const {
  return this->cend();
}

aint::const_reverse_iterator aint::crbegin() const {
  return const_reverse_iterator(this->blocks_, this->size());
}

aint::const_reverse_iterator aint::crend() const {
  return const_reverse_iterator(nullptr, 0);
}

aint::const_reverse_iterator aint::rbegin() const {
  return this->crbegin();
}

aint::const_reverse_iterator aint::rend() const {
  return this->crend();
}

/**
 * IO STREAMS
 */
std::ostream &operator<<(std::ostream &o, const aint &ai) {
  auto str = ai.to_string();
  o << str;
  free(str);
  return o;
}
