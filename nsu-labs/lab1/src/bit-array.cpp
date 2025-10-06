#include "bit-array.h"
#include <cmath>
#include <limits>
#include <stdexcept>

// Private

bool BitArray::is_full() { return bits % elem_bits == 0; }

// Public

BitArray::BitArray() { bits = 0; }

BitArray::~BitArray() { buffer.clear(); }

BitArray::BitArray(int num_bits, unsigned long value) {
  int size = ceil((float)num_bits / elem_bits);

  buffer.resize(size);
  buffer.assign(size, value);

  bits = num_bits;
}

BitArray::BitArray(const BitArray &b) {
  bits = b.bits;
  buffer = std::vector(b.buffer);
}

void BitArray::swap(BitArray &b) {
  buffer.swap(b.buffer);
  unsigned int swapped = b.bits;
  b.bits = bits;
  bits = swapped;
}

void BitArray::resize(int num_bits, bool value) {
  if (num_bits < 0) {
    throw std::invalid_argument("Unable to resize: num_bits is negarive");
  }

  if (num_bits == 0) {
    buffer.clear();
    buffer.resize(0);
    bits = 0;
    return;
  }

  if (num_bits >= std::numeric_limits<int>::max()) {
    throw std::out_of_range("Unable to resize: num_bits is too large");
  }

  int size = ceil((float)num_bits / elem_bits);
  buffer.resize(size);

  unsigned int old_bits = bits;
  bits = num_bits;

  for (int i = old_bits; i < bits; i++) {
    set(i, value);
  }
}

void BitArray::clear() {
  buffer.clear();
  bits = 0;
}

void BitArray::push_back(bool bit) {
  if (buffer.empty()) {
    buffer.resize(default_size);
  }

  if (is_full()) {
    buffer.resize(ceil((float)buffer.size() * resize_mul));
  }

  bits++;
  set(bits - 1, bit);
}

BitArray &BitArray::set(int n, bool val) {
  if (n < 0) {
    throw std::invalid_argument("Unable to set: n is negative");
  }

  if ((unsigned int)n >= bits) {
    throw std::out_of_range("Unable to set: n is out of range");
  }

  int elem_pos = n / elem_bits;
  int bit_pos = n % elem_bits;

  unsigned long elem = buffer.at(elem_pos);
  unsigned long mask = 1UL << bit_pos;

  buffer[elem_pos] = val ? elem | mask : elem & ~mask;

  return *this;
}

BitArray &BitArray::set() {
  buffer.assign(buffer.size(), ULONG_MAX);
  return *this;
}

BitArray &BitArray::reset(int n) {
  this->set(n, false);
  return *this;
}

BitArray &BitArray::reset() {
  buffer.assign(buffer.capacity(), 0);
  return *this;
}

bool BitArray::any() const {
  int size = buffer.size();
  if (size <= 0) {
    return false;
  }

  for (int i = 0; i < size - 1; i++) {
    if (buffer.at(i) > 0)
      return true;
  }

  unsigned long elem = buffer.at(size - 1);
  for (int i = 0; i < bits % elem_bits; i++) {
    if (elem & 1UL)
      return true;
    elem >>= 1;
  }

  return false;
}

bool BitArray::none() const { return !this->any(); }

int BitArray::count() const {
  int count = 0;
  int pos = 0;
  unsigned long elem;

  for (unsigned int i = 0; i < bits; i++) {
    if (i % elem_bits == 0) {
      elem = buffer.at(pos++);
    }

    count += elem & 1UL;
    elem >>= 1;
  }

  return count;
}

int BitArray::size() const { return bits; }

bool BitArray::empty() const { return bits == 0; }

std::string BitArray::to_string() const {
  std::string str(bits, '0');
  int pos = 0;
  unsigned long elem;

  for (unsigned int i = 0; i < bits; i++) {
    if (i % elem_bits == 0) {
      elem = buffer.at(pos++);
    }

    str[bits - i - 1] = elem & 1UL ? '1' : '0';
    elem >>= 1;
  }

  return str;
}

bool BitArray::operator[](int i) const {
  if (i >= bits) {
    throw std::out_of_range("Out of range trying to access [i]th bit");
  }

  int elem_pos = i / elem_bits;
  int bit_pos = i % elem_bits;

  return (buffer.at(elem_pos) >> bit_pos) & 1UL;
}

BitArray &BitArray::operator=(const BitArray &b) {
  if (this != &b) {
    buffer = b.buffer;
    bits = b.bits;
  }
  return *this;
}

BitArray &BitArray::operator&=(const BitArray &b) {
  if (this == &b) {
    return *this;
  }

  if (bits != b.bits) {
    throw std::invalid_argument(
        "BitArrays must have the same size for &= operator");
  }

  int size = ceil((float)bits / elem_bits);
  for (int i = 0; i < size; i++) {
    buffer[i] &= b.buffer[i];
  }

  return *this;
}

BitArray &BitArray::operator|=(const BitArray &b) {
  if (this == &b) {
    return *this;
  }

  if (bits != b.bits) {
    throw std::invalid_argument(
        "BitArrays must have the same size for |= operator");
  }

  int size = ceil((float)bits / elem_bits);
  for (int i = 0; i < size; i++) {
    buffer[i] |= b.buffer[i];
  }

  return *this;
}

BitArray &BitArray::operator^=(const BitArray &b) {
  if (bits != b.bits) {
    throw std::invalid_argument(
        "BitArrays must have the same size for ^= operator");
  }

  int size = ceil((float)bits / elem_bits);
  for (int i = 0; i < size; i++) {
    buffer[i] ^= b.buffer[i];
  }

  return *this;
}

BitArray &BitArray::operator<<=(int n) {
  if (n < 0) {
    throw std::invalid_argument("Unable to <<= for negative n");
  }

  if (n == 0) {
    return *this;
  }

  if (n > std::numeric_limits<int>::max() - bits) {
    throw std::out_of_range("BitArray bitwise >>= shift amount is too large");
  }

  unsigned int new_bits = bits + n;

  int bit_shift = new_bits % elem_bits;
  int elem_shift = n / elem_bits;

  this->resize(new_bits);

  for (int i = new_bits - 1; i >= n; i--) {
    this->set(i, (*this)[i - n]);
  }
  for (int i = n - 1; i >= 0; i--) {
    this->set(i, false);
  }

  bits = new_bits;

  return *this;
}

BitArray &BitArray::operator>>=(int n) {
  if (n < 0) {
    throw std::invalid_argument("Unable to >>= for negative n");
  }

  if (n == 0) {
    return *this;
  }

  if (n >= bits) {
    this->reset();
    this->resize(0);
    return *this;
  }

  unsigned int new_bits = bits - n;

  for (int i = 0; i < new_bits; i++) {
    this->set(i, (*this)[i + n]);
  }

  this->resize(new_bits);

  return *this;
}

BitArray BitArray::operator<<(int n) const { return BitArray(*this) <<= n; }

BitArray BitArray::operator>>(int n) const { return BitArray(*this) >>= n; }

// Functions

bool operator==(const BitArray &b1, const BitArray &b2) {
  if (b1.size() != b2.size()) {
    return false;
  }

  for (int i = 0; i < b1.size(); i++) {
    if (b1[i] != b2[i]) {
      return false;
    }
  }

  return true;
}

bool operator!=(const BitArray &b1, const BitArray &b2) {
  if (b1.size() != b2.size()) {
    return true;
  }

  for (int i = 0; i < b1.size(); i++) {
    if (b1[i] == b2[i]) {
      return false;
    }
  }

  return true;
}

BitArray operator&(const BitArray &b1, const BitArray &b2) {
  return BitArray(b1) &= b2;
}

BitArray operator|(const BitArray &b1, const BitArray &b2) {
  return BitArray(b1) |= b2;
}

BitArray operator^(const BitArray &b1, const BitArray &b2) {
  return BitArray(b1) ^= b2;
}
