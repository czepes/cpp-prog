#ifndef RENDER
#define RENDER

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

#define DEAD ' '
#define ALIVE 'O'

#include "simulator.h"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace std;

class Render {
private:
  Cells &cells;
  ostream &canvas;

public:
  inline static vector<char> graphics{DEAD, ALIVE};

  Render(Cells &cells, ostream &output = cout);
  Render(Cells &cells, ofstream &output);

  void render(const string &name = "");
  void render(ostream &output, const string &name = "");

  static void render(Cells &cells, const string &name, ostream &output) {
    if (!name.empty()) {
      output << name << endl;
    }
    for (const auto &row : cells) {
      for (const auto &cell : row) {
        output << graphics[cell];
      }
      output << endl;
    }
  }
  void clean();
};

#endif
