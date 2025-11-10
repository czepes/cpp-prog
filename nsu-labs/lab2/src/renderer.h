#ifndef RENDER
#define RENDER

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
  const vector<char> graphics{'.', 'O'};
  const pair<int, int> size;
  ostream &canvas;

public:
  Renderer(const pair<int, int> size, ostream &output = cout);
  Renderer(const pair<int, int> size, ofstream &output);

  void render(Cells &cells, const string &name = "");
  void render(ostream &output, Cells &cells, const string &name = "");
};

#endif
