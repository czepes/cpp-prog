#include "../src/csv-parser.hpp"
#include "../src/tuple-print.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

using namespace std;

// Тестовый CSV файл для тестов
const string TEST_CSV_CONTENT =
    "1;Alice;25.5\n2;Bob;30.0\n3;Charlie;22.7\n4;Diana;35.2";

// Функция для создания временного файла
string create_temp_csv(const string &content) {
  string filename = "test_temp.csv";
  ofstream file(filename);
  file << content;
  file.close();
  return filename;
}

// ========== CSV PARSER TESTS ==========

TEST(CSVParserTest, BasicParsing) {
  string filename = create_temp_csv(TEST_CSV_CONTENT);

  CSVParser<int, string, double> parser(filename, 0, ';');

  vector<tuple<int, string, double>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 4);
  EXPECT_EQ(results[0], make_tuple(1, "Alice", 25.5));
  EXPECT_EQ(results[1], make_tuple(2, "Bob", 30.0));
  EXPECT_EQ(results[2], make_tuple(3, "Charlie", 22.7));
  EXPECT_EQ(results[3], make_tuple(4, "Diana", 35.2));

  filesystem::remove(filename);
}

// Тест 2: Пропуск строк
TEST(CSVParserTest, SkipLines) {
  string content =
      "header1;header2;header3\ncomment;ignore;this\n1;Alice;25.5\n2;Bob;30.0";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 2, ';');
  vector<tuple<int, string, double>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 2);
  EXPECT_EQ(results[0], make_tuple(1, "Alice", 25.5));

  filesystem::remove(filename);
}

// Тест 3: Пустые строки
TEST(CSVParserTest, EmptyLines) {
  string content = "1;Alice;25.5\n\n2;Bob;30.0\n\n3;Charlie;22.7\n\n";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';');

  vector<tuple<int, string, double>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 3);

  filesystem::remove(filename);
}

// Тест 4: Кавычки
TEST(CSVParserTest, WithQuotes) {
  string content = "1;'Alice Smith';25.5\n2;'Bob ''The "
                   "Builder''';30.0\n3;'Charlie, Jr.';22.7";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';', '\'');

  vector<tuple<int, string, double>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 3);
  EXPECT_EQ(get<1>(results[1]), "Bob \'The Builder\'");

  filesystem::remove(filename);
}

// Тест 5: Другой разделитель
TEST(CSVParserTest, DifferentDelimiter) {
  string content = "1,Alice,25.5\n2,Bob,30.0\n3,Charlie,22.7";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ',');

  vector<tuple<int, string, double>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 3);

  filesystem::remove(filename);
}

// Тест 6: Исключения при неверном количестве колонок
TEST(CSVParserTest, WrongNumberOfColumns) {
  string content = "1;Alice\n2;Bob;30.0;extra\n3;Charlie;22.7";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';');

  EXPECT_THROW(
      {
        for (const auto &row : parser) {
          // Просто пытаемся пройтись по всем строкам
        }
      },
      CSVError);

  filesystem::remove(filename);
}

// Тест 7: Исключения при конвертации типов
TEST(CSVParserTest, TypeConversionError) {
  string content = "1;Alice;25.5\nnot_a_number;Bob;30.0\n3;Charlie;22.7";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';');

  auto it = parser.begin();
  EXPECT_NO_THROW(*it); // Первая строка должна быть OK

  EXPECT_THROW(++it, CSVError); // Вторая строка должна бросить исключение

  filesystem::remove(filename);
}

// Тест 8: Итераторы
TEST(CSVParserTest, IteratorOperations) {
  string filename = create_temp_csv(TEST_CSV_CONTENT);
  CSVParser<int, string, double> parser(filename, 0, ';');

  auto it = parser.begin();
  auto end = parser.end();

  EXPECT_NE(it, end);

  // Проверяем постинкремент
  auto it2 = it++;
  EXPECT_EQ(*it2, make_tuple(1, "Alice", 25.5));
  EXPECT_EQ(*it, make_tuple(2, "Bob", 30.0));

  // Проверяем преинкремент
  ++it;
  EXPECT_EQ(*it, make_tuple(3, "Charlie", 22.7));

  CSVParser<int, string, double> skipped(filename, 10, ';');
  EXPECT_EQ(skipped.begin(), skipped.end());

  filesystem::remove(filename);
}

TEST(CSVParserTest, VariousTypes) {
  string content = "1;Alice;25.5;1000;3.14\n2;Bob;30.0;2000;2.71";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double, long, float> parser(filename, 0, ';');

  vector<tuple<int, string, double, long, float>> results;
  for (const auto &row : parser) {
    results.push_back(row);
  }

  ASSERT_EQ(results.size(), 2);
  EXPECT_FLOAT_EQ(get<4>(results[0]), 3.14f);

  filesystem::remove(filename);
}

// ========== ERRORS TESTS ==========

TEST(CSVErrorTest, ErrorMessages) {
  // Тест неверного количества кавычек
  string content = "1;\"Alice;25.5\n2;Bob;30.0";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';', '"');

  try {
    for (const auto &row : parser) {
      // Просто проходим по строкам
    }
    FAIL() << "Expected CSVError exception";
  } catch (const CSVError &e) {
    EXPECT_EQ(e.get_line_number(), 1);
    EXPECT_NE(string(e.what()).find("Quotes are not closed"), string::npos);
  }

  filesystem::remove(filename);
}

TEST(CSVErrorTest, MidQuoteError) {
  // Кавычка в середине поля без кавычек
  string content = "1;Ali\"ce;25.5";

  string filename = create_temp_csv(content);
  CSVParser<int, string, double> parser(filename, 0, ';', '"');

  try {
    for (const auto &row : parser) {
      // Просто проходим по строкам
    }
    FAIL() << "Expected CSVError exception";
  } catch (const CSVError &e) {
    EXPECT_EQ(e.get_line_number(), 1);
    EXPECT_NE(string(e.what()).find("Quote character at the middle"),
              string::npos);
  }

  filesystem::remove(filename);
}

// ========== TUPLE PRINT TESTS ==========

TEST(TuplePrintTest, BasicPrinting) {
  tuple<int, string, double> t(1, "Alice", 25.5);
  stringstream ss;
  ss << t;

  EXPECT_EQ(ss.str(), "(1, Alice, 25.5)");
}

TEST(TuplePrintTest, EmptyTuple) {
  tuple<> empty;
  stringstream ss;
  ss << empty;

  EXPECT_EQ(ss.str(), "()");
}

TEST(TuplePrintTest, SingleElement) {
  tuple<int> single(42);
  stringstream ss;
  ss << single;

  EXPECT_EQ(ss.str(), "(42)");
}

TEST(TuplePrintTest, MultipleTypes) {
  tuple<int, string, double, bool, char> t(1, "test", 3.14, true, 'A');
  stringstream ss;
  ss << t;

  EXPECT_EQ(ss.str(), "(1, test, 3.14, 1, A)");
}

TEST(TuplePrintTest, NestedOutput) {
  tuple<int, string> t1(1, "Alice");
  tuple<double, bool> t2(3.14, true);

  stringstream ss;
  ss << "First: " << t1 << ", Second: " << t2;

  EXPECT_EQ(ss.str(), "First: (1, Alice), Second: (3.14, 1)");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
