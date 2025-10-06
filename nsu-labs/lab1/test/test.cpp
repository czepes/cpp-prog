#include "../src/bit-array.h"
#include <climits>
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

class BitArrayTest : public testing::Test {
protected:
  BitArray *ba_empty;
  BitArray *ba_orig;
  BitArray *ba_copy;
  BitArray *ba_long;

  static constexpr int ba_orig_bits = CHAR_WIDTH;
  static constexpr unsigned long ba_orig_value = UCHAR_MAX;

  static constexpr int ba_long_bits = LONG_WIDTH * 4;
  static constexpr unsigned long ba_long_value = ULONG_MAX;

  BitArrayTest() {
    ba_empty = new BitArray();
    ba_orig = new BitArray(ba_orig_bits, ba_orig_value);
    ba_copy = new BitArray(*ba_orig);
    ba_long = new BitArray(ba_long_bits, ba_long_value);
  }

  ~BitArrayTest() {
    delete ba_empty;
    delete ba_orig;
    delete ba_copy;
    delete ba_long;
  }
};

TEST_F(BitArrayTest, AreEqualOnConstructCopy) {
  EXPECT_EQ(ba_orig->size(), ba_copy->size());
  EXPECT_EQ(ba_orig->to_string(), ba_copy->to_string());
}

TEST_F(BitArrayTest, ToString) {
  EXPECT_EQ(ba_empty->to_string(), "");
  EXPECT_EQ(ba_orig->to_string(), std::string(ba_orig_bits, '1'));
  EXPECT_EQ(ba_long->to_string(), std::string(ba_long_bits, '1'));
}

TEST_F(BitArrayTest, Count) {
  EXPECT_EQ(ba_empty->count(), 0);
  EXPECT_EQ(ba_orig->count(), 8);
  EXPECT_EQ(ba_copy->count(), 8);
  EXPECT_EQ(ba_long->count(), 256);
}

TEST_F(BitArrayTest, Any) {
  EXPECT_EQ(ba_empty->any(), false);
  EXPECT_EQ(ba_orig->any(), true);
  EXPECT_EQ(ba_copy->any(), true);
  EXPECT_EQ(ba_long->any(), true);
}

TEST_F(BitArrayTest, None) {
  EXPECT_EQ(ba_empty->none(), true);
  EXPECT_EQ(ba_orig->none(), false);
  EXPECT_EQ(ba_copy->none(), false);
  EXPECT_EQ(ba_long->none(), false);
}

TEST_F(BitArrayTest, Empty) {
  EXPECT_EQ(ba_empty->empty(), true);
  EXPECT_EQ(ba_orig->empty(), false);
  ba_empty->push_back(false);
  EXPECT_EQ(ba_empty->empty(), false);
}

TEST_F(BitArrayTest, AtOperator) {
  EXPECT_THROW((*ba_empty)[0], std::out_of_range);
  EXPECT_THROW((*ba_orig)[ba_orig_bits], std::out_of_range);
  EXPECT_THROW((*ba_orig)[-1], std::out_of_range);

  EXPECT_EQ((*ba_orig)[0], true);
  EXPECT_EQ((*ba_orig)[ba_orig_bits - 1], true);

  ba_orig->set(false, 0);
  EXPECT_EQ((*ba_orig)[0], false);
}

TEST_F(BitArrayTest, Clear) {
  ba_long->clear();
  EXPECT_EQ(ba_long->any(), false);
}

TEST_F(BitArrayTest, SetReset) {
  ba_orig->reset(0);
  EXPECT_EQ((*ba_orig)[0], false);
  ba_orig->reset();
  EXPECT_EQ(ba_orig->none(), true);
  ba_orig->set(ba_orig_bits - 1, true);
  EXPECT_EQ((*ba_orig)[ba_orig_bits - 1], true);
  ba_orig->set();
  EXPECT_EQ(ba_orig->count(), ba_orig_bits);

  ba_empty->resize(64);
  ba_empty->set(0, true);
  ba_empty->set(31, true);
  ba_empty->set(32, true);
  ba_empty->set(63, true);

  EXPECT_EQ(ba_empty->count(), 4);

  EXPECT_TRUE((*ba_empty)[0]);
  EXPECT_TRUE((*ba_empty)[31]);
  EXPECT_TRUE((*ba_empty)[32]);
  EXPECT_TRUE((*ba_empty)[63]);

  ba_empty->resize(0);

  EXPECT_THROW(ba_empty->set(0), std::out_of_range);
  EXPECT_THROW(ba_empty->set(-1), std::invalid_argument);
  EXPECT_THROW(ba_empty->reset(0), std::out_of_range);
  EXPECT_THROW(ba_empty->reset(-1), std::invalid_argument);
}

TEST_F(BitArrayTest, PushBack) {
  ba_empty->push_back(1);
  ba_empty->push_back(0);
  ba_empty->push_back(1);
  ba_empty->push_back(0);
  ba_empty->push_back(1);

  EXPECT_EQ(ba_empty->to_string(), "10101");

  ba_empty->clear();

  for (int i = 0; i <= BitArray::elem_bits; i++) {
    ba_empty->push_back(true);
  }

  EXPECT_EQ(ba_empty->count(), BitArray::elem_bits + 1);
}

TEST_F(BitArrayTest, Swap) {
  ba_empty->swap(*ba_orig);

  EXPECT_EQ(ba_empty->count(), ba_orig_bits);
  EXPECT_EQ(ba_orig->count(), 0);

  ba_empty->swap(*ba_orig);

  EXPECT_EQ(ba_empty->count(), 0);
  EXPECT_EQ(ba_orig->count(), ba_orig_bits);
}

TEST_F(BitArrayTest, AssignmentOperator) {
  *ba_empty = *ba_orig;

  EXPECT_EQ(ba_empty->to_string(), ba_orig->to_string());

  ba_empty->reset();

  EXPECT_NE(ba_empty->to_string(), ba_orig->to_string());

  BitArray *ba_ptr = ba_orig;
  *ba_orig = *ba_ptr;

  EXPECT_EQ(ba_orig, ba_ptr);
}

TEST_F(BitArrayTest, Resize) {
  ba_orig->resize(8, false);

  EXPECT_EQ(ba_orig->size(), 8);
  EXPECT_EQ(ba_orig->count(), 8);

  ba_orig->resize(16, false);

  EXPECT_EQ(ba_orig->size(), 16);
  EXPECT_EQ(ba_orig->count(), 8);

  ba_orig->resize(24, true);

  EXPECT_EQ(ba_orig->size(), 24);
  EXPECT_EQ(ba_orig->count(), 16);

  ba_orig->resize(0);

  EXPECT_EQ(ba_orig->size(), 0);
  EXPECT_EQ(ba_orig->count(), 0);

  ba_orig->resize(7);
  ba_orig->push_back(true);

  EXPECT_EQ(ba_orig->to_string(), "10000000");

  EXPECT_THROW(ba_orig->resize(-1), std::invalid_argument);
  EXPECT_THROW(ba_orig->resize(std::numeric_limits<int>::max()),
               std::out_of_range);
}

TEST_F(BitArrayTest, EqualityOperators) {
  EXPECT_TRUE(*ba_empty != *ba_orig);
  EXPECT_FALSE(*ba_empty == *ba_orig);

  ba_empty->resize(ba_orig_bits);
  EXPECT_FALSE(*ba_empty == *ba_orig);
  EXPECT_TRUE(*ba_empty != *ba_orig);
  EXPECT_FALSE(*ba_orig != *ba_copy);

  EXPECT_TRUE(*ba_orig == *ba_copy);
  *ba_empty = *ba_copy;
  EXPECT_TRUE(*ba_empty == *ba_orig);
}

TEST_F(BitArrayTest, BitAssignOperators) {
  // Exceptions

  EXPECT_THROW((*ba_empty) &= (*ba_orig), std::invalid_argument);
  EXPECT_THROW((*ba_orig) |= (*ba_empty), std::invalid_argument);
  EXPECT_THROW((*ba_long) ^= (*ba_orig), std::invalid_argument);

  // Self

  *ba_orig &= *ba_orig;
  EXPECT_EQ(ba_orig->count(), ba_orig->count());

  *ba_orig |= *ba_orig;
  EXPECT_EQ(ba_orig->count(), ba_orig->count());

  *ba_long ^= *ba_long;
  EXPECT_EQ(ba_long->count(), 0);

  // Combinations

  ba_empty->resize(ba_orig_bits);

  *ba_empty &= *ba_orig;
  EXPECT_TRUE(ba_empty->none());

  *ba_copy &= *ba_empty;
  EXPECT_TRUE(ba_copy->none());

  *ba_empty |= *ba_orig;
  EXPECT_EQ(ba_empty->count(), ba_orig->count());

  *ba_copy ^= *ba_orig;
  EXPECT_EQ(ba_copy->count(), ba_orig->count());

  *ba_copy ^= *ba_orig;
  EXPECT_TRUE(ba_copy->none());
}

TEST_F(BitArrayTest, BitOperators) {
  ba_empty->resize(ba_orig_bits);
  BitArray ba_new = (*ba_empty) | (*ba_orig);

  EXPECT_EQ(ba_new, *ba_orig);

  ba_new = (*ba_empty) & ba_new;

  EXPECT_EQ(ba_new, *ba_empty);

  ba_new = (*ba_orig) ^ (*ba_orig);

  EXPECT_EQ(ba_new, *ba_empty);
}

TEST_F(BitArrayTest, BitShiftAssignOperators) {
  ba_empty->push_back(true);

  *ba_empty <<= 5;

  EXPECT_EQ(ba_empty->to_string(), "100000");

  *ba_empty >>= 5;

  EXPECT_EQ(ba_empty->to_string(), "1");

  *ba_empty <<= 128;

  EXPECT_EQ(ba_empty->size(), 129);
  EXPECT_EQ(ba_empty->count(), 1);
  EXPECT_TRUE((*ba_empty)[128]);

  EXPECT_THROW((*ba_empty) <<= -1, std::invalid_argument);
  EXPECT_THROW((*ba_empty) >>= -1, std::invalid_argument);
  EXPECT_THROW((*ba_empty) <<= INT_MAX, std::out_of_range);
}

TEST_F(BitArrayTest, BitShiftOperators) {
  int ba_new_bits = ba_orig_bits / 2;
  BitArray ba_new = *ba_orig >> ba_new_bits;

  EXPECT_EQ(ba_new.size(), ba_new_bits);
  EXPECT_EQ(ba_new.count(), ba_new_bits);

  BitArray ba_new_2 = ba_new << ba_new_bits;

  EXPECT_EQ(ba_new_2.size(), ba_orig_bits);
  EXPECT_EQ(ba_new_2.count(), ba_new_bits);

  ba_new_2 = ba_new << 0;

  EXPECT_EQ(ba_new, ba_new_2);

  ba_new_2 = ba_new >> 0;

  EXPECT_EQ(ba_new, ba_new_2);

  ba_new_2 = ba_new >> std::numeric_limits<int>::max();

  EXPECT_TRUE(ba_new_2.empty());
}

TEST_F(BitArrayTest, ConstIterator) {
  for (const auto bit : *ba_orig) {
    EXPECT_TRUE(bit);
  }

  ba_orig->reset();

  for (const auto bit : *ba_orig) {
    EXPECT_FALSE(bit);
  }
}
