#include "Str.hpp"

Str::Str() {
  this->s_ = nullptr;
}
Str::~Str() {
  this->clear();
}

Str::Str(const Str& str) {
  this->s_ = nullptr;
  this->assign(str.s_);
}

Str::Str(const char* other) {
  this->s_ = nullptr;
  this->assign(other);
}

Str::Str(Str&& other) noexcept
        :s_(other.s_) {
  other.s_ = nullptr;
}

Str& Str::operator=(const char* str) {
  this->assign(str);
  return *this;
}

Str& Str::operator=(const Str& str) {
  this->assign(str.s_);
  return *this;
}

Str& Str::operator=(Str&& str) noexcept {
  std::swap(this->s_, str.s_);
  return *this;
}

Str Str::operator+(const Str& b) {
  Str res;
  if (!this->s_ || !b.s_) {
    return res;
  }

  size_t bufferSize = length() + b.length() + 1;
  res.s_ = new char[bufferSize];

  char* o = res.s_;
  if (s_)
    for (char* i = s_; *i; i++, o++) {
      *o = *i;
    }
  if (b.s_)
    for (char* i = b.s_; *i; i++, o++) {
      *o = *i;
    }

  *o = 0;

  return (res);
}

char& Str::operator[](size_t index) {
  return this->s_[index];
}

const char& Str::operator[](size_t index) const {
  return this->s_[index];
}

size_t Str::length() const {
  size_t length = 0;
  if (!this->s_) {
    return 0;
  }

  for (; this->s_[length]; ++length);

  return length;
}

void Str::clear() {
  delete this->s_;
  this->s_ = nullptr;
}

void Str::assign(const char* text) {
  this->clear();
  if (!text) {
    return;
  }

  size_t length;

  for (length = 0; text[length]; ++length);

  s_ = new char[length + 1]{ 0 };

  for (size_t i = 0; i < length; ++i) {
    this->s_[i] = text[i];
  }
}

Str::iterator Str::begin() const {
  return iterator(this->s_);
}

Str::iterator Str::end() const {
  return iterator(nullptr);
}

Str::iterator::iterator(const iterator& other) {
  this->ptr_ = other.ptr_;
}

Str::iterator::iterator(char* c) {
  this->ptr_ = c;
}

char& Str::iterator::operator*() {
  return *this->ptr_;
}

bool Str::iterator::operator==(const iterator& other) {
  return this->ptr_ == other.ptr_;
}

bool Str::iterator::operator!=(const iterator& other) {
  return this->ptr_ != other.ptr_;
}

Str::iterator& Str::iterator::operator++() {
  this->ptr_++;

  if (!*this->ptr_) {
    this->ptr_ = nullptr;
  }
  return *this;
}

std::ostream& operator<<(std::ostream& o, const Str& str) {
  for (char& i : str) {
    o << i;
  }
  return o;
}
