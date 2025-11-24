#include "bit-array.h"
#include <limits>
#include <stdexcept>

// Private

int BitArray::to_bytes(int bits) {
  return bits / byte_bits + (bits % byte_bits ? 1 : 0);
}

// Public

BitArray::BitArray() : bits(0) {}

BitArray::~BitArray() { bytes.clear(); }

BitArray::BitArray(int num_bits, byte_type value) {
  const int size = to_bytes(num_bits);

  bytes.resize(size);
  bytes.assign(size, value);

  bits = num_bits;
}

BitArray::BitArray(const BitArray &b)
    : bits(b.bits), bytes(std::vector(b.bytes)) {}

void BitArray::swap(BitArray &b) {
  bytes.swap(b.bytes);
  const unsigned int tmp = b.bits;
  b.bits = bits;
  bits = tmp;
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

  const int size = to_bytes(num_bits);
  if (size != bytes.size()) {
    bytes.resize(size);
  }

  const unsigned int old_bits = bits;
  bits = num_bits;

  if (!value) {
    return;
  }

  for (int i = old_bits; i < bits; i++) {
    set(i, true);
  }
}

void BitArray::clear() {
  bytes.clear();
  bits = 0;
}

void BitArray::push_back(bool bit) {
  resize(bits + 1);
  set(bits - 1, bit);
}

BitArray &BitArray::set(int n, bool val) {
  if (n < 0) {
    throw std::invalid_argument("Unable to set: n is negative");
  }

  if ((unsigned int)n >= bits) {
    throw std::out_of_range("Unable to set: n is out of range");
  }

  const int byte_pos = n / byte_bits;
  const int bit_pos = n % byte_bits;

  const byte_type byte = bytes.at(byte_pos);
  const byte_type mask = 1UL << bit_pos;

  bytes[byte_pos] = val ? byte | mask : byte & ~mask;

  return *this;
}

bool BitArray::get(int i) const {
  if (i < 0) {
    throw std::invalid_argument("Unable to get: n is negative");
  }

  if ((unsigned int)i >= bits) {
    throw std::out_of_range("Unable to set: n is out of range");
  }

  const int byte_pos = i / byte_bits;
  const int bit_pos = i % byte_bits;
  return (bytes.at(byte_pos) >> bit_pos) & 1UL;
}

BitArray &BitArray::set() {
  const int size = bytes.size();
  const int trail = bits % byte_bits;

  if (size > 0) {
    bytes.assign(size, std::numeric_limits<byte_type>::max());
  }
  if (trail > 0) {
    bytes[size - 1] = (1UL << trail) - 1;
  }

  return *this;
}

BitArray &BitArray::reset(int n) {
  set(n, false);
  return *this;
}

BitArray &BitArray::reset() {
  bytes.assign(bytes.size(), 0);
  return *this;
}

bool BitArray::any() const {
  const int size = bytes.size();

  if (size <= 0) {
    return false;
  }

  for (const byte_type byte : bytes) {
    if (byte > 0) {
      return true;
    }
  }

  return false;
}

bool BitArray::none() const { return !any(); }

int BitArray::count() const {
  int count = 0;

  for (const byte_type byte : bytes) {
    byte_type cur = byte;

    while (cur > 0) {
      count += cur & 1UL;
      cur >>= 1;
    }
  }

  return count;
}

int BitArray::size() const { return bits; }

bool BitArray::empty() const { return bits == 0; }

std::string BitArray::to_string() const {
  std::string str(bits, '0');
  int pos = bits - 1;

  for (const bool bit : *this) {
    str[pos--] = bit ? '1' : '0';
  }

  return str;
}

bool BitArray::operator[](int i) const {
  if (i < 0 || i >= bits) {
    throw std::out_of_range("Out of range trying to access [i]th bit");
  }

  return get(i);
}

BitArray::BitProxy BitArray::operator[](int i) {
  if (i < 0 || i >= bits) {
    throw std::out_of_range("Out of range trying to access [i]th bit");
  }
  return BitProxy(*this, i);
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

  const int size = bytes.size();

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

  const int size = bytes.size();

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

  const int size = bytes.size();

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
    throw std::out_of_range("BitArray bitwise >>= n is too large");
  }

  resize(bits + n);

  const int bit_shift = n % byte_bits;
  const int byte_shift = n / byte_bits;
  const int size = bytes.size();

  if (byte_shift > 0) {
    for (int i = size - 1; i >= 0; i--) {
      bytes[i] = i >= byte_shift ? bytes[i - byte_shift] : 0UL;
    }
  }

  if (bit_shift > 0) {
    const int inv_shift = byte_bits - bit_shift;

    for (int i = size - 1; i > byte_shift; i--) {
      bytes[i] = (bytes[i] << bit_shift) | (bytes[i - 1] >> inv_shift);
    }

    bytes[byte_shift] <<= bit_shift;
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
    reset();
    resize(0);
    return *this;
  }

  const int bit_shift = n % byte_bits;
  const int byte_shift = n / byte_bits;
  const int new_size = bytes.size() - byte_shift;

  if (byte_shift > 0) {
    for (int i = 0; i < new_size; i++) {
      bytes[i] = bytes[i + byte_shift];
    }
  }

  if (bit_shift > 0) {
    const int inv_shift = byte_bits - bit_shift;

    for (int i = 0; i < new_size - 1; i++) {
      bytes[i] = (bytes[i] >> bit_shift) | (bytes[i + 1] << inv_shift);
    }

    bytes[new_size - 1] >>= bit_shift;
  }

  resize(bits - n);

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

// Proxy

BitArray::BitProxy::BitProxy(BitArray &ba, int idx) : ba(ba), idx(idx) {}

BitArray::BitProxy &BitArray::BitProxy::operator=(bool bit) {
  ba.set(idx, bit);
  return *this;
}

BitArray::BitProxy::operator bool() const { return ba.get(idx); };

// Iterators

BitArray::ConstIterator::ConstIterator(const BitArray &ba, int idx)
    : ba(ba), idx(idx) {
  byte = idx >= ba.bits ? 0 : ba.bytes[idx / byte_bits];
  byte >>= idx % byte_bits;
}

bool BitArray::ConstIterator::operator*() { return byte & 1UL; }

BitArray::ConstIterator &BitArray::ConstIterator::operator++() {
  ++idx;
  byte >>= 1;

  const int bit_pos = idx % byte_bits;
  const int byte_pos = idx / byte_bits;

  if (bit_pos == 0 && byte_pos < ba.bytes.size()) {
    byte = ba.bytes[byte_pos];
  }

  return *this;
}

BitArray::ConstIterator BitArray::ConstIterator::operator++(int) {
  ConstIterator temp = *this;
  ++(*this);
  return temp;
}

bool BitArray::ConstIterator::operator==(
    const BitArray::ConstIterator &other) const {
  return ba == other.ba && idx == other.idx;
}

bool BitArray::ConstIterator::operator!=(
    const BitArray::ConstIterator &other) const {
  return !(*this == other);
}

BitArray::ConstIterator BitArray::begin() const {
  return ConstIterator(*this, 0);
};
BitArray::ConstIterator BitArray::end() const {
  return ConstIterator(*this, bits);
};

BitArray::Iterator::Iterator(BitArray &ba, int idx) : ba(ba), idx(idx) {};

BitArray::BitProxy BitArray::Iterator::operator*() { return BitProxy(ba, idx); }

BitArray::Iterator &BitArray::Iterator::operator++() {
  idx++;
  return *this;
}

BitArray::Iterator BitArray::Iterator::operator++(int) {
  Iterator temp = *this;
  ++(*this);
  return temp;
}

bool BitArray::Iterator::operator==(const BitArray::Iterator &other) const {
  return ba == other.ba && idx == other.idx;
}

bool BitArray::Iterator::operator!=(const BitArray::Iterator &other) const {
  return !(*this == other);
}

BitArray::Iterator BitArray::begin() { return Iterator(*this, 0); };
BitArray::Iterator BitArray::end() { return Iterator(*this, bits); };
