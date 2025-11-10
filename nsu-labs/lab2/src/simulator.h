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
  string name;
  string survival_rule;
  string birth_rule;
  Cells cells;

  void parse_lifefile(ifstream &in);
  int count_neighbours(int y, int x);

public:
  Simulator();
  Simulator(ifstream &in);

  int get_height() const;
  int get_width() const;
  const pair<int, int> get_size() const;

  void live(int n);
  void live();

  const string get_name() const;
  Cells &get_cells();
};

void put_block(Cells &cells, int y, int x);
void put_beehive(Cells &cells, int y, int x);
void put_loaf(Cells &cells, int y, int x);
void put_boat(Cells &cells, int y, int x);
void put_tub(Cells &cells, int y, int x);
void put_blinker(Cells &cells, int y, int x);
void put_toad(Cells &cells, int y, int x);
void put_beacon(Cells &cells, int y, int x);
void put_pulsar(Cells &cells, int y, int x);
void put_pentadecathlon(Cells &cells, int y, int x);
void put_glider(Cells &cells, int y, int x);
void put_lwss(Cells &cells, int y, int x);
void put_mwss(Cells &cells, int y, int x);
void put_hwss(Cells &cells, int y, int x);

#endif
