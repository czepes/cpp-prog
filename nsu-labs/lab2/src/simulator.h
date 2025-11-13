#ifndef SIMULATOR
#define SIMULATOR

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

using namespace std;

int norm(int v, int n);
const pair<int, int> get_window_size();

using celmat = vector<vector<bool>>;

class Cell {
private:
  celmat *ptr;
  int y, x;

public:
  Cell(vector<vector<bool>> *ptr, int y, int x) : ptr(ptr), y(y), x(x) {};
  operator bool() const;
  Cell &operator=(bool alive);
};

class CellsRow {
private:
  celmat *ptr;
  int y;

public:
  CellsRow(vector<vector<bool>> *ptr, int y) : ptr(ptr), y(y) {};
  Cell operator[](int x);
};

class Cells {
private:
  celmat matrix;

public:
  pair<int, int> size;

  Cells();
  Cells(const pair<int, int> size);
  CellsRow operator[](int y);
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

  int count_neighbours(int y, int x);
  bool check_rule(const string &values);

public:
  Simulator();
  Simulator(ifstream &in);
  Simulator(const pair<int, int> size);
  Simulator(const pair<int, int> size, ifstream &in);

  const pair<int, int> get_size() const;

  void live(int n);
  void live();

  string get_name() const;
  string get_survival_rule() const;
  string get_birth_rule() const;
  Cells &get_cells();

  void set_name(string name);
  void set_survival_rule(string rule);
  void set_birth_rule(string rule);
};

#endif
