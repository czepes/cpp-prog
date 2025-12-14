#include "simulator.h"
#include "patterns.h"
#include <stdexcept>

// Functions

int norm(int v, int n) {
  while (v < 0) {
    v += n;
  }
  v %= n;
  return v;
}

const pair<int, int> get_window_size() {
#ifdef ONSOLE_SCREEN_BUFFER_INFO
  ONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
  int height{0};
  int width{0};
  struct winsize window;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == 0) {
    height = window.ws_row;
    width = window.ws_col;
  }
#endif

  if (height <= 0 || width <= 0) {
    const char *rows = getenv("LINES");
    const char *cols = getenv("COLUMNS");
    if (rows) {
      height = stoi(rows);
    }
    if (cols) {
      width = stoi(cols);
    }
  }

  if (height <= 0) {
    height = 24;
  }
  if (width <= 0) {
    width = 80;
  }

  return pair(height, width);
}

// Cells

Cell::operator bool() const { return (*ptr)[y][x]; }
Cell &Cell::operator=(bool alive) {
  (*ptr)[y][x] = alive;
  return *this;
}
ConstCell::operator bool() const { return (*ptr)[y][x]; };

Cell CellsRow::operator[](int x) {
  return Cell(ptr, y, norm(x, (*ptr)[y].size()));
};

ConstCell ConstCellsRow::operator[](int x) const {
  return ConstCell(ptr, y, norm(x, (*ptr)[y].size()));
};

Cells::Cells() : size(pair(0, 0)) {};
Cells::Cells(const pair<int, int> size) : size(size) {
  matrix = vector(size.first, vector(size.second, false));
}

CellsRow Cells::operator[](int y) {
  return CellsRow(&matrix, norm(y, size.first));
};
ConstCellsRow Cells::operator[](int y) const {
  return ConstCellsRow(&matrix, norm(y, size.first));
};

const pair<int, int> Cells::get_size() const { return size; }

void Cells::clear() { matrix = vector(size.first, vector(size.second, false)); }

// Simulator

Simulator::Simulator(bool fill) {
  generate_cells();

  if (!fill) {
    return;
  }

  // At least 11 height, 38 width
  if (put_glider_gun(cells, 0, 0)) {
    return;
  }

  // At least 7 height, 9 width
  if (put_hwss(cells, 0, 0)) {
    return;
  }

  // At least 5 height, 5 width
  if (put_glider(cells, 0, 0)) {
    return;
  }

  // At least 2 height, 2 width
  if (put_block(cells, 0, 0)) {
    return;
  }
};

Simulator::Simulator(ifstream &in) {
  generate_cells();
  parse_lif(in);
};

Simulator::Simulator(const pair<int, int> size) { generate_cells(size); }

Simulator::Simulator(const pair<int, int> size, ifstream &in) {
  generate_cells(size);
  parse_lif(in);
}

void Simulator::generate_cells() {
  pair<int, int> window_size = get_window_size();
  // Reserving space for simulation's name and prompt
  window_size.first -= 4;
  cells = Cells(window_size);
}

void Simulator::generate_cells(const pair<int, int> size) {
  cells = Cells(size);
}

bool Simulator::check_rule(const string &values) const {
  const string allowed{"12345678"};

  for (const char num : values) {
    if (allowed.find(num) == string::npos) {
      return false;
    }
  }

  // If values are empty, returns true!
  return true;
}

void Simulator::parse_lif(ifstream &input) {
  const string error{"Simulator parsing error\n"};
  string buf;
  string name;
  string birth_rule;
  string survival_rule;
  Cells cells(this->cells.get_size());
  int line{0};

  // #Life 1.06
  getline(input, buf);
  line++;

  if (buf != "#Life 1.06") {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file has wrong format");
  }

  // #N Simulation Name
  getline(input, buf);
  line++;

  if (buf.empty() || buf.find("#N ") == string::npos) {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file has wrong name");
  }
  name = buf.substr(3, buf.length() - 3);

  // #R B{0-8}/S{0-8}
  getline(input, buf);
  line++;

  if (buf.empty()) {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file is missing rules");
  }
  if (buf.find("#R ") == string::npos) {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file has wrong rules format");
  }
  if (buf.find("B") == string::npos) {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file is missing birth rules");
  }
  if (buf.find("S") == string::npos) {
    throw invalid_argument(error + "Line " + to_string(line) +
                           ": Input file is missing survival rules");
  }

  stringstream rules(buf.substr(3, buf.length() - 3), ios_base::in);

  while (getline(rules, buf, '\\')) {
    char rule = buf[0];
    string values = buf.substr(1, buf.length() - 1);

    switch (rule) {
    case 'B':
      if (check_rule(values)) {
        birth_rule = values;
      } else {
        throw invalid_argument(error + "Line " + to_string(line) +
                               ": Wrong birth rule values " + values);
      }
      break;
    case 'S':
      if (check_rule(values)) {
        survival_rule = values;
      } else {
        throw invalid_argument(error + "Line " + to_string(line) +
                               ": Wrong survival rule values " + values);
      }
      break;
    default:
      throw invalid_argument(error + "Line " + to_string(line) +
                             ": Wrong rule " + rule);
    }
  }

  // x y
  bool empty{true};
  int x, y;
  while (getline(input, buf)) {
    line++;

    stringstream coords(buf, ios_base::in);

    coords >> x;
    if (coords.fail()) {
      throw invalid_argument(error + "Line " + to_string(line) +
                             ": Wrong coordinates " + buf);
    }

    coords >> y;
    if (coords.fail()) {
      throw invalid_argument(error + "Line " + to_string(line) +
                             ": Wrong coordinates " + buf);
    }

    cells[y][x] = true;
    empty = false;
  }

  if (empty) {
    throw invalid_argument(error + "EOF: Missing coordinates");
  }

  this->name = name;
  this->birth_rule = birth_rule;
  this->survival_rule = survival_rule;
  this->cells = cells;
}

int Simulator::count_neighbours(int y, int x) const {
  int count = 0;

  for (int sy = -1; sy <= 1; sy++) {
    for (int sx = -1; sx <= 1; sx++) {
      if (sy == 0 && sx == 0) {
        continue;
      }
      if (cells[y + sy][x + sx]) {
        count++;
      }
    }
  }

  return count;
}

void Simulator::live(int n) {
  while (n > 0) {
    live();
    n--;
  }
}
void Simulator::live() {
  pair<int, int> size = cells.get_size();
  Cells new_cells(size);

  for (int y = 0; y < size.first; y++) {
    for (int x = 0; x < size.second; x++) {
      char neighbours = count_neighbours(y, x) + '0';

      if (cells[y][x]) {
        size_t pos = survival_rule.find(neighbours);
        new_cells[y][x] = survival_rule.find(neighbours) != string::npos;
      } else {
        size_t pos = birth_rule.find(neighbours);
        new_cells[y][x] = pos != string::npos;
      }
    }
  }

  cells = new_cells;
}

string Simulator::get_name() const { return name; };
string Simulator::get_birth_rule() const { return birth_rule; };
string Simulator::get_survival_rule() const { return survival_rule; };
Cells &Simulator::get_cells() { return cells; }

void Simulator::set_name(string name) { this->name = name; }

void Simulator::set_birth_rule(string rule) {
  if (check_rule(rule)) {
    birth_rule = rule;
  }
}

void Simulator::set_survival_rule(string rule) {
  if (check_rule(rule)) {
    survival_rule = rule;
  }
}

Simulator &Simulator::operator>>(ofstream &output) {
  if (!output.is_open()) {
    return *this;
  }

  pair<int, int> size = cells.get_size();
  output << "#Life 1.06" << endl
         << "#N " << name << endl
         << "#R B" << birth_rule << "\\S" << survival_rule << endl;

  for (int y = 0; y < size.first; y++) {
    for (int x = 0; x < size.second; x++) {
      if (cells[y][x]) {
        output << x << " " << y << endl;
      }
    }
  }

  return *this;
}

Simulator &Simulator::operator<<(ifstream &input) {
  if (!input.is_open()) {
    return *this;
  }

  parse_lif(input);

  return *this;
}
