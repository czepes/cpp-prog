#ifndef RENDER
#define RENDER

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

#include "simulator.h"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace std;

class Renderer {
private:
  const vector<char> graphics{' ', 'O'};
  Cells &cells;
  ostream &canvas;

public:
  Renderer(Cells &cells, ostream &output = cout);
  Renderer(Cells &cells, ofstream &output);

  void render(const string &name = "");
  void render(ostream &output, const string &name = "");
  void clean();
};

#endif
