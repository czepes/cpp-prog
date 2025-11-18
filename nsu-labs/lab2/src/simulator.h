#ifndef SIMULATOR
#define SIMULATOR

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include "windows.h"
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

using namespace std;
using celrow = vector<bool>;
using celmat = vector<celrow>;

int norm(int v, int n);
const pair<int, int> get_window_size();

class Cell {
private:
  celmat *ptr;
  int y, x;

public:
  Cell(celmat *ptr, int y, int x) : ptr(ptr), y(y), x(x) {};
  operator bool() const;
  Cell &operator=(bool alive);
};

class CellsRow {
private:
  celmat *ptr;
  int y;

public:
  CellsRow(celmat *ptr, int y) : ptr(ptr), y(y) {};
  Cell operator[](int x);

  using iterator = celrow::iterator;
  using const_iterator = celrow::const_iterator;

  iterator begin() { return (*ptr)[y].begin(); };
  iterator end() { return (*ptr)[y].end(); };
  const_iterator begin() const { return (*ptr)[y].begin(); };
  const_iterator end() const { return (*ptr)[y].end(); };
  const_iterator cbegin() const { return (*ptr)[y].cbegin(); };
  const_iterator cend() const { return (*ptr)[y].cend(); };
};

class ConstCell {
private:
  const celmat *ptr;
  int y, x;

public:
  ConstCell(const celmat *ptr, int y, int x) : ptr(ptr), y(y), x(x) {};
  operator bool() const;
};

class ConstCellsRow {
private:
  const celmat *ptr;
  int y;

public:
  ConstCellsRow(const celmat *ptr, int y) : ptr(ptr), y(y) {};
  ConstCell operator[](int x) const;
};

class Cells {
private:
  celmat matrix;
  pair<int, int> size;

public:
  Cells();
  Cells(const pair<int, int> size);
  CellsRow operator[](int y);
  ConstCellsRow operator[](int y) const;

  const pair<int, int> get_size() const;

  void clear();

  using iterator = celmat::iterator;
  using const_iterator = celmat::const_iterator;

  iterator begin() { return matrix.begin(); };
  iterator end() { return matrix.end(); };
  const_iterator begin() const { return matrix.begin(); };
  const_iterator end() const { return matrix.end(); };
  const_iterator cbegin() const { return matrix.cbegin(); };
  const_iterator cend() const { return matrix.cend(); };
};

class Simulator {
private:
  string name{"Conway's Game of Life"};
  string survival_rule{"23"};
  string birth_rule{"3"};
  Cells cells;

  void parse_lif(ifstream &in);
  void generate_cells();
  void generate_cells(const pair<int, int> size);

  int count_neighbours(int y, int x) const;
  bool check_rule(const string &values) const;

public:
  Simulator(bool fill = false);
  Simulator(ifstream &in);
  Simulator(const pair<int, int> size);
  Simulator(const pair<int, int> size, ifstream &in);

  void live(int n);
  void live();

  string get_name() const;
  string get_survival_rule() const;
  string get_birth_rule() const;
  Cells &get_cells();

  void set_name(string name);
  void set_survival_rule(string rule);
  void set_birth_rule(string rule);

  Simulator &operator>>(ifstream &input);
  Simulator &operator<<(ofstream &output);
};

#endif
