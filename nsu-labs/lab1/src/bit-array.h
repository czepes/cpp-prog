#ifndef BIT_ARRAY
#define BIT_ARRAY

#include <climits>
#include <cstdint>
#include <string>
#include <vector>

#define BYTE_SIZE 8
#define BIT_ARRAY_DEFAULT_SIZE 1

// TODO: Iterator

class BitArray {
private:
  static constexpr int default_size = BIT_ARRAY_DEFAULT_SIZE;
  static constexpr float resize_mul = 1.5;

  std::vector<unsigned long> buffer;
  unsigned int bits;

  bool is_full();
  bool elem_is_full();
  int get_elems_amount();

public:
  static constexpr int elem_bits = sizeof(unsigned long) * BYTE_SIZE;

  BitArray();
  ~BitArray();

  // Construct array, with specified amount of bits
  // First sizeof(long) bits may be initialized with parameter 'value'
  explicit BitArray(int num_bits, unsigned long value = 0);
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
  // TODO: Error message and handling
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

  int size() const;
  bool empty() const;

  // Return string representation of bit array
  std::string to_string() const;

  // Iterators

  class const_iterator {
  private:
    const BitArray *ba;
    unsigned int idx;

  public:
    const_iterator(const BitArray *ba, int idx) : ba(ba), idx(idx) {}

    bool operator*() { return (*ba)[idx]; }

    const_iterator &operator++() {
      ++idx;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator temp = *this;
      idx++;
      return temp;
    }

    bool operator==(const const_iterator &other) const {
      return ba == other.ba && idx == other.idx;
    }

    bool operator!=(const const_iterator &other) const {
      return !(*this == other);
    }
  };

  const_iterator begin() const { return const_iterator(this, 0); };
  const_iterator end() const { return const_iterator(this, bits); };
  const_iterator cbegin() const { return const_iterator(this, 0); };
  const_iterator cend() const { return const_iterator(this, bits); };
};

bool operator==(const BitArray &b1, const BitArray &b2);
bool operator!=(const BitArray &b1, const BitArray &b2);

BitArray operator&(const BitArray &b1, const BitArray &b2);
BitArray operator|(const BitArray &b1, const BitArray &b2);
BitArray operator^(const BitArray &b1, const BitArray &b2);

#endif
