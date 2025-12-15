#include "csv-parser.hpp"
#include "student.hpp"
#include "tuple-print.hpp"
#include <fstream>
#include <iostream>

using namespace std;

int main(void) {
  ifstream in("../data/data.csv");
  CSVParser<int, Student> parser(in, 1);

  for (const auto &rs : parser) {
    cout << rs << endl;
  }

  return 0;
}
