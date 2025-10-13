#ifndef BIT_ARRAY
#define BIT_ARRAY

#include <climits>
#include <cstdint>
#include <string>
#include <vector>

#define BYTE_SIZE 8
#define BIT_ARRAY_DEFAULT_SIZE 1

using byte_type = unsigned long;

class BitArray {
private:
  static constexpr int default_size = BIT_ARRAY_DEFAULT_SIZE;

  std::vector<byte_type> bytes;
  unsigned int bits;

  static int to_bytes(int bits);

public:
  static constexpr int byte_bits = sizeof(byte_type) * BYTE_SIZE;

  // Proxy

  class BitProxy {
  private:
    unsigned int idx;
    BitArray &ba;

  public:
    BitProxy(BitArray &ba, int idx);
    BitProxy &operator=(bool bit);
    operator bool() const;
  };

  // Iterators

  class ConstIterator {
  private:
    const BitArray &ba;
    unsigned int idx;
    byte_type byte;

  public:
    ConstIterator(const BitArray &ba, int idx);

    bool operator*();

    ConstIterator &operator++();
    ConstIterator operator++(int);

    bool operator==(const ConstIterator &other) const;
    bool operator!=(const ConstIterator &other) const;
  };

  class Iterator {
  private:
    BitArray &ba;
    unsigned int idx;

  public:
    Iterator(BitArray &ba, int idx);

    BitProxy operator*();

    Iterator &operator++();
    Iterator operator++(int);

    bool operator==(const Iterator &other) const;
    bool operator!=(const Iterator &other) const;
  };

  BitArray();
  ~BitArray();

  // Construct array, with specified amount of bits
  // First sizeof(long) bits may be initialized with parameter 'value'
  explicit BitArray(int num_bits, byte_type value = 0);
  BitArray(const BitArray &b);

  // Replace values of 2 bit arrays
  void swap(BitArray &b);

  BitArray &operator=(const BitArray &b);

  // Resize bit array
  // If array expands, new elements are initialized with 'value'
  void resize(int num_bits, bool value = false);

  // Clear bit array
  void clear();

  // Add 1 bit at the end of bit array
  // Reallocate memory if needed
  void push_back(bool bit);

  // Bit operators for bit array
  // Can be used only for array of the same size
  BitArray &operator&=(const BitArray &b);
  BitArray &operator|=(const BitArray &b);
  BitArray &operator^=(const BitArray &b);

  // Bitwise shifting, filling with 0's
  BitArray &operator<<=(int n);
  BitArray &operator>>=(int n);
  BitArray operator<<(int n) const;
  BitArray operator>>(int n) const;

  // Set n'th bit to 'value'
  BitArray &set(int n, bool val = true);

  // Fill array with 1's
  BitArray &set();

  // Set n'th bit to 0
  BitArray &reset(int n);

  // Fill array with 0's
  BitArray &reset();

  // True, if at least one bit of value 1
  bool any() const;

  // True, if all bits are 0's
  bool none() const;

  // Bit inversion
  BitArray operator~() const;

  // Count bits of value 1
  int count() const;

  // Return i'th bit value
  bool operator[](int i) const;
  BitProxy operator[](int i);

  int size() const;
  bool empty() const;

  // Return string representation of bit array
  std::string to_string() const;

  ConstIterator begin() const;
  ConstIterator end() const;

  Iterator begin();
  Iterator end();
};

bool operator==(const BitArray &b1, const BitArray &b2);
bool operator!=(const BitArray &b1, const BitArray &b2);

BitArray operator&(const BitArray &b1, const BitArray &b2);
BitArray operator|(const BitArray &b1, const BitArray &b2);
BitArray operator^(const BitArray &b1, const BitArray &b2);

#endif
