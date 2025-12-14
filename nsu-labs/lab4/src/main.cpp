#include "csv-parser.hpp"
#include "tuple-print.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(void) {
  ifstream in("../data/data.csv");
  CSVParser<int, string> parser(in, 1);

  for (const auto &rs : parser) {
    cout << rs << endl;
  }

  return 0;
}
