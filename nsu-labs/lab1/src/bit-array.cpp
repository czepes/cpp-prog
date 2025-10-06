#include "bit-array.h"
#include <cmath>
#include <limits>
#include <stdexcept>

// Private

bool BitArray::is_full() { return bits % byte_bits == 0; }

// Public

BitArray::BitArray() { bits = 0; }

BitArray::~BitArray() { bytes.clear(); }

BitArray::BitArray(int num_bits, unsigned long value) {
  int size = ceil((float)num_bits / byte_bits);

  bytes.resize(size);
  bytes.assign(size, value);

  bits = num_bits;
}

BitArray::BitArray(const BitArray &b) {
  bits = b.bits;
  bytes = std::vector(b.bytes);
}

void BitArray::swap(BitArray &b) {
  bytes.swap(b.bytes);
  unsigned int swapped = b.bits;
  b.bits = bits;
  bits = swapped;
}

void BitArray::resize(int num_bits, bool value) {
  if (num_bits < 0) {
    throw std::invalid_argument("Unable to resize: num_bits is negarive");
  }

  if (num_bits == 0) {
    bytes.clear();
    bytes.resize(0);
    bits = 0;
    return;
  }

  if (num_bits >= std::numeric_limits<int>::max()) {
    throw std::out_of_range("Unable to resize: num_bits is too large");
  }

  const int size = ceil((float)num_bits / byte_bits);
  bytes.resize(size);

  const unsigned int old_bits = bits;
  bits = num_bits;

  for (int i = old_bits; i < bits; i++) {
    set(i, value);
  }
}

void BitArray::clear() {
  bytes.clear();
  bits = 0;
}

void BitArray::push_back(bool bit) {
  if (bytes.empty()) {
    bytes.resize(default_size);
  }

  if (is_full()) {
    bytes.resize(ceil((float)bytes.size() * resize_mul));
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

  int byte_pos = n / byte_bits;
  int bit_pos = n % byte_bits;

  unsigned long byte = bytes.at(byte_pos);
  unsigned long mask = 1UL << bit_pos;

  bytes[byte_pos] = val ? byte | mask : byte & ~mask;

  return *this;
}

BitArray &BitArray::set() {
  bytes.assign(bytes.size(), std::numeric_limits<unsigned long>::max());
  return *this;
}

BitArray &BitArray::reset(int n) {
  this->set(n, false);
  return *this;
}

BitArray &BitArray::reset() {
  bytes.assign(bytes.capacity(), 0);
  return *this;
}

bool BitArray::any() const {
  int size = bytes.size();
  if (size <= 0) {
    return false;
  }

  for (int i = 0; i < size - 1; i++) {
    if (bytes.at(i) > 0)
      return true;
  }

  unsigned long byte = bytes.at(size - 1);
  for (int i = 0; i < bits % byte_bits; i++) {
    if (byte & 1UL) {
      return true;
    }
    byte >>= 1;
  }

  return false;
}

bool BitArray::none() const { return !this->any(); }

int BitArray::count() const {
  int count = 0;
  int pos = 0;
  unsigned long byte;

  for (unsigned int i = 0; i < bits; i++) {
    if (i % byte_bits == 0) {
      byte = bytes.at(pos++);
    }

    count += byte & 1UL;
    byte >>= 1;
  }

  return count;
}

int BitArray::size() const { return bits; }

bool BitArray::empty() const { return bits == 0; }

std::string BitArray::to_string() const {
  std::string str(bits, '0');
  int pos = 0;
  unsigned long byte;

  for (unsigned int i = 0; i < bits; i++) {
    if (i % byte_bits == 0) {
      byte = bytes.at(pos++);
    }

    str[bits - i - 1] = byte & 1UL ? '1' : '0';
    byte >>= 1;
  }

  return str;
}

bool BitArray::operator[](int i) const {
  if (i >= bits) {
    throw std::out_of_range("Out of range trying to access [i]th bit");
  }

  int byte_pos = i / byte_bits;
  int bit_pos = i % byte_bits;

  return (bytes.at(byte_pos) >> bit_pos) & 1UL;
}

BitArray &BitArray::operator=(const BitArray &b) {
  if (this != &b) {
    bytes = b.bytes;
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

  int size = ceil((float)bits / byte_bits);
  for (int i = 0; i < size; i++) {
    bytes[i] &= b.bytes[i];
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

  int size = ceil((float)bits / byte_bits);
  for (int i = 0; i < size; i++) {
    bytes[i] |= b.bytes[i];
  }

  return *this;
}

BitArray &BitArray::operator^=(const BitArray &b) {
  if (bits != b.bits) {
    throw std::invalid_argument(
        "BitArrays must have the same size for ^= operator");
  }

  int size = ceil((float)bits / byte_bits);
  for (int i = 0; i < size; i++) {
    bytes[i] ^= b.bytes[i];
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

  this->resize(bits + n);

  const int bit_shift = n % byte_bits;
  const int byte_shift = n / byte_bits;
  const int last = bytes.size() - 1;

  if (byte_shift > 0) {
    for (int i = last; i >= 0; i--) {
      bytes[i] = i >= byte_shift ? bytes[i - byte_shift] : 0;
    }
  }

  if (bit_shift > 0) {
    for (int i = last; i >= byte_shift; i--) {
      unsigned long mask = 0;

      if (i > byte_shift) {
        unsigned int shift = byte_bits - bit_shift;
        mask = (~0UL << shift & bytes[i - 1]) >> shift;
      }

      bytes[i] = bytes[i] << bit_shift | mask;
    }
  }

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

  const int bit_shift = n % byte_bits;
  const int byte_shift = n / byte_bits;
  const int size = bytes.size() - byte_shift;

  if (byte_shift > 0) {
    for (int i = 0; i < size; i++) {
      bytes[i] = bytes[i + byte_shift];
    }
  }

  if (bit_shift > 0) {
    for (int i = 0; i < size; i++) {
      unsigned long mask = 0;

      if (i < size - 1) {
        unsigned int shift = byte_bits - bit_shift;
        mask = (~0UL >> shift & bytes[i + 1]) << shift;
      }

      bytes[i] = bytes[i] >> bit_shift | mask;
    }
  }

  this->resize(bits - n);

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
