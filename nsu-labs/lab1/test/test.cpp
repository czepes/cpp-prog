#include "../src/bit-array.h"
#include <climits>
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

class BitArrayTest : public testing::Test {
protected:
  BitArray *ba_empty;
  BitArray *ba_char;
  BitArray *ba_copy;
  BitArray *ba_long;

  static constexpr int ba_char_bits = CHAR_WIDTH;
  static constexpr unsigned long ba_char_value = UCHAR_MAX;

  static constexpr int ba_long_bits = LONG_WIDTH * 4;
  static constexpr unsigned long ba_long_value = ULONG_MAX;

  BitArrayTest() {
    ba_empty = new BitArray();
    ba_char = new BitArray(ba_char_bits, ba_char_value);
    ba_copy = new BitArray(*ba_char);
    ba_long = new BitArray(ba_long_bits, ba_long_value);
  }

  ~BitArrayTest() {
    delete ba_empty;
    delete ba_char;
    delete ba_copy;
    delete ba_long;
  }
};

TEST_F(BitArrayTest, AreEqualOnConstructCopy) {
  BitArray &ba1 = *ba_char;
  BitArray &ba2 = *ba_copy;

  EXPECT_EQ(ba1.size(), ba2.size());
  EXPECT_EQ(ba1.to_string(), ba2.to_string());
  EXPECT_EQ(ba1, ba2);
}

TEST_F(BitArrayTest, ToString) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_long;

  EXPECT_EQ(ba1.to_string(), "");
  EXPECT_EQ(ba2.to_string(), std::string(ba2.size(), '1'));
  EXPECT_EQ(ba3.to_string(), std::string(ba3.size(), '1'));

  const int new_size = 100;
  std::string str(new_size, '0');

  ba1.resize(new_size);

  for (int i = 1; i < new_size; i += 2) {
    ba1.set(i);
    str[new_size - i - 1] = '1';
  }

  EXPECT_EQ(ba1.to_string(), str);
}

TEST_F(BitArrayTest, Count) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_copy;
  BitArray &ba4 = *ba_long;

  EXPECT_EQ(ba1.count(), 0);
  EXPECT_EQ(ba2.count(), 8);
  EXPECT_EQ(ba3.count(), 8);
  EXPECT_EQ(ba4.count(), 256);
}

TEST_F(BitArrayTest, Any) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_copy;
  BitArray &ba4 = *ba_long;

  EXPECT_FALSE(ba1.any());
  EXPECT_TRUE(ba2.any());
  EXPECT_TRUE(ba3.any());
  EXPECT_TRUE(ba4.any());
}

TEST_F(BitArrayTest, None) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_copy;
  BitArray &ba4 = *ba_long;

  EXPECT_TRUE(ba1.none());
  EXPECT_FALSE(ba2.none());
  EXPECT_FALSE(ba3.none());
  EXPECT_FALSE(ba4.none());
}

TEST_F(BitArrayTest, Empty) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  EXPECT_TRUE(ba1.empty());
  EXPECT_FALSE(ba2.empty());

  ba1.push_back(false);

  EXPECT_FALSE(ba1.empty());
}

TEST_F(BitArrayTest, AtOperator) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  EXPECT_THROW(ba1[0], std::out_of_range);
  EXPECT_THROW(ba2[ba2.size()], std::out_of_range);
  EXPECT_THROW(ba2[-1], std::out_of_range);

  EXPECT_TRUE(ba2[0]);
  EXPECT_TRUE(ba2[ba2.size() - 1]);

  ba2.set(false, 0);
  EXPECT_FALSE(ba2[0]);
}

TEST_F(BitArrayTest, Clear) {
  BitArray &ba = *ba_long;

  ba.clear();
  EXPECT_FALSE(ba.any());
  EXPECT_TRUE(ba.empty());
}

TEST_F(BitArrayTest, SetReset) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_long;

  EXPECT_THROW(ba1.set(0), std::out_of_range);
  EXPECT_THROW(ba1.set(-1), std::invalid_argument);
  EXPECT_THROW(ba1.reset(0), std::out_of_range);
  EXPECT_THROW(ba1.reset(-1), std::invalid_argument);

  ba2.reset(0);
  EXPECT_FALSE(ba2[0]);
  ba2.reset();
  EXPECT_TRUE(ba2.none());
  ba2.set(ba_char_bits - 1, true);
  EXPECT_TRUE(ba2[ba_char_bits - 1]);
  ba2.set();
  EXPECT_EQ(ba2.count(), ba_char_bits);

  ba1.resize(64);
  ba1.set(0, true).set(31, true).set(32, true).set(63, true);

  EXPECT_EQ(ba1.count(), 4);

  EXPECT_TRUE(ba1[0]);
  EXPECT_TRUE(ba1[31]);
  EXPECT_TRUE(ba1[32]);
  EXPECT_TRUE(ba1[63]);

  ba3.reset();

  EXPECT_FALSE(ba3.any());

  ba3.set();

  EXPECT_EQ(ba3.count(), ba3.size());
}

TEST_F(BitArrayTest, PushBack) {
  BitArray &ba = *ba_empty;

  ba.push_back(1);
  ba.push_back(0);
  ba.push_back(1);
  ba.push_back(0);
  ba.push_back(1);

  EXPECT_EQ(ba.to_string(), "10101");

  ba.clear();

  for (int i = 0; i <= BitArray::byte_bits; i++) {
    ba.push_back(true);
  }

  EXPECT_EQ(ba.count(), BitArray::byte_bits + 1);
}

TEST_F(BitArrayTest, Swap) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  EXPECT_EQ(ba1.count(), 0);
  EXPECT_EQ(ba2.count(), ba2.size());

  ba1.swap(ba2);

  EXPECT_EQ(ba1.count(), ba1.size());
  EXPECT_EQ(ba2.count(), 0);

  ba2.swap(ba1);

  EXPECT_EQ(ba1.count(), 0);
  EXPECT_EQ(ba2.count(), ba2.size());
}

TEST_F(BitArrayTest, AssignmentOperator) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  *ba_empty = *ba_char;

  EXPECT_EQ(ba1.to_string(), ba2.to_string());

  ba1.reset();

  EXPECT_NE(ba1.to_string(), ba2.to_string());

  BitArray ba3 = ba2;

  EXPECT_EQ(ba2, ba3);
}

TEST_F(BitArrayTest, Resize) {
  BitArray &ba = *ba_char;

  ba.resize(8, false);

  EXPECT_EQ(ba.size(), 8);
  EXPECT_EQ(ba.count(), 8);

  ba.resize(16, false);

  EXPECT_EQ(ba.size(), 16);
  EXPECT_EQ(ba.count(), 8);

  ba.resize(24, true);

  EXPECT_EQ(ba.size(), 24);
  EXPECT_EQ(ba.count(), 16);

  ba.resize(0);

  EXPECT_EQ(ba.size(), 0);
  EXPECT_EQ(ba.count(), 0);

  ba.resize(7);
  ba.push_back(true);

  EXPECT_EQ(ba.to_string(), "10000000");

  EXPECT_THROW(ba.resize(-1), std::invalid_argument);
  EXPECT_THROW(ba.resize(std::numeric_limits<int>::max()), std::out_of_range);
}

TEST_F(BitArrayTest, EqualityOperators) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_copy;

  EXPECT_TRUE(ba1 != ba2);
  EXPECT_FALSE(ba1 == ba2);

  EXPECT_TRUE(ba2 == ba3);
  EXPECT_FALSE(ba2 != ba3);

  ba1.resize(ba2.size());

  EXPECT_FALSE(ba1 == ba2);
  EXPECT_TRUE(ba1 != ba2);

  ba1.set();

  EXPECT_TRUE(ba1 == ba2);
  EXPECT_TRUE(ba1 == ba3);
  EXPECT_FALSE(ba1 != ba2);
  EXPECT_FALSE(ba1 != ba3);
}

TEST_F(BitArrayTest, BitAssignOperators) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;
  BitArray &ba3 = *ba_copy;
  BitArray &ba4 = *ba_long;

  // Exceptions

  EXPECT_THROW(ba1 &= ba2, std::invalid_argument);
  EXPECT_THROW(ba2 |= ba1, std::invalid_argument);
  EXPECT_THROW(ba4 ^= ba2, std::invalid_argument);

  // Self

  ba2 &= ba2;
  EXPECT_EQ(ba2.count(), ba2.size());

  ba2 |= ba2;
  EXPECT_EQ(ba2.count(), ba2.size());

  ba4 ^= ba4;
  EXPECT_EQ(ba4.count(), 0);

  // Combinations

  ba1.resize(ba2.size());

  ba1 &= ba2;
  EXPECT_TRUE(ba1.none());

  ba3 &= ba1;
  EXPECT_TRUE(ba3.none());

  ba1 |= ba2;
  EXPECT_EQ(ba1.count(), ba2.count());

  ba3 ^= ba2;
  EXPECT_EQ(ba3.count(), ba2.count());

  ba3 ^= ba2;
  EXPECT_TRUE(ba3.none());
}

TEST_F(BitArrayTest, BitOperators) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  ba1.resize(ba2.size());
  BitArray ba3 = ba1 | ba2;

  EXPECT_EQ(ba3, ba2);

  ba3 = ba1 & ba3;

  EXPECT_EQ(ba3, ba1);

  ba3 = ba2 ^ ba2;

  EXPECT_EQ(ba3, ba1);
}

TEST_F(BitArrayTest, BitShiftAssignOperators) {
  BitArray &ba1 = *ba_empty;
  BitArray &ba2 = *ba_char;

  ba1.push_back(true);

  ba1 <<= 5;

  EXPECT_EQ(ba1.to_string(), "100000");

  ba1 >>= 5;

  EXPECT_EQ(ba1.to_string(), "1");

  ba1 <<= 32;
  ba1.set(1).set(0);
  ba1 <<= 64;
  ba1.set(2).set(1).set(0);

  EXPECT_EQ(ba1.size(), 97);
  EXPECT_EQ(ba1.count(), 6);
  EXPECT_TRUE(ba1[96]);
  EXPECT_FALSE(ba1[95]);
  EXPECT_TRUE(ba1[65]);
  EXPECT_TRUE(ba1[64]);
  EXPECT_FALSE(ba1[63]);
  EXPECT_FALSE(ba1[3]);
  EXPECT_TRUE(ba1[2]);
  EXPECT_TRUE(ba1[1]);
  EXPECT_TRUE(ba1[0]);

  ba1 <<= 127;

  EXPECT_EQ(ba1.size(), 97 + 127);
  EXPECT_EQ(ba1.count(), 6);

  ba1 >>= 127 + 2;

  EXPECT_EQ(ba1.size(), 95);
  EXPECT_EQ(ba1.count(), 4);
  EXPECT_TRUE(ba1[94]);
  EXPECT_FALSE(ba1[93]);
  EXPECT_TRUE(ba1[63]);
  EXPECT_TRUE(ba1[62]);
  EXPECT_FALSE(ba1[61]);
  EXPECT_FALSE(ba1[1]);
  EXPECT_TRUE(ba1[0]);

  ba1 >>= 70;

  EXPECT_EQ(ba1.size(), 25);
  EXPECT_EQ(ba1.count(), 1);
  EXPECT_TRUE(ba1[24]);
  EXPECT_FALSE(ba1[23]);

  ba1 >>= 25;

  EXPECT_TRUE(ba1.empty());

  EXPECT_THROW(ba2 <<= -1, std::invalid_argument);
  EXPECT_THROW(ba2 >>= -1, std::invalid_argument);
  EXPECT_THROW(ba2 <<= INT_MAX, std::out_of_range);
}

TEST_F(BitArrayTest, BitShiftOperators) {
  BitArray &ba1 = *ba_char;
  int shift = ba1.size() / 2;

  BitArray ba2 = ba1 >> shift;

  EXPECT_EQ(ba2.size(), shift);
  EXPECT_EQ(ba2.count(), shift);

  BitArray ba3 = ba2 << shift;

  EXPECT_EQ(ba3.size(), ba1.size());
  EXPECT_EQ(ba3.count(), ba2.count());

  ba3 = ba2 << 0;

  EXPECT_EQ(ba2, ba3);

  ba3 = ba2 >> 0;

  EXPECT_EQ(ba2, ba3);

  ba3 = ba2 >> std::numeric_limits<int>::max();

  EXPECT_TRUE(ba3.empty());
}

TEST_F(BitArrayTest, ConstIterator) {
  BitArray &ba = *ba_char;

  for (const bool bit : ba) {
    EXPECT_TRUE(bit);
  }

  ba.reset();

  for (const bool bit : ba) {
    EXPECT_FALSE(bit);
  }

  BitArray::const_iterator iter = ba.begin();
  while (iter != ba.end()) {
    EXPECT_FALSE(*(iter++));
  }

  // Even bits are 1, odd bits are 0
  for (int i = 0; i < ba.size(); i += 2) {
    ba.set(i);
  }

  int i = 0;
  for (const bool bit : ba) {
    if (i % 2) {
      EXPECT_FALSE(bit);
    } else {
      EXPECT_TRUE(bit);
    }
    i++;
  }
}
