#include "simulator.h"

int norm(int v, int n) {
  while (v < 0) {
    v += n;
  }
  v %= n;
  return v;
}

const pair<int, int> get_window_size() {
  int height{0};
  int width{0};
  struct winsize window;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == 0) {
    height = window.ws_row;
    width = window.ws_col;
  }

  if (height <= 0 || width <= 0) {
    const char *rows = getenv("LINES");
    const char *cols = getenv("COLUMNS");
    if (rows) {
      height = atoi(rows);
    }
    if (cols) {
      width = atoi(cols);
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

Cell::operator bool() const { return (*ptr)[y][x]; }
Cell &Cell::operator=(bool alive) {
  (*ptr)[y][x] = alive;
  return *this;
}

Cell CellsRow::operator[](int x) {
  return Cell(ptr, y, norm(x, (*ptr)[y].size()));
};

Cells ::Cells() : size(pair(0, 0)) {};
Cells::Cells(const pair<int, int> size) : size(size) {
  matrix = vector(size.first, vector(size.second, false));
}
CellsRow Cells::operator[](int y) {
  return CellsRow(&matrix, norm(y, matrix.size()));
};

Simulator::Simulator() {
  name = "Conway's Game of Life";
  birth_rule = "3";
  survival_rule = "23";

  pair<int, int> window_size = get_window_size();
  // Reserving space for simulation's name and prompt
  window_size.first -= 4;
  cells = Cells(window_size);

  // Still lifes start: 0, 0 size: 25x6
  put_block(cells, 0, 0);
  put_beehive(cells, 4, 0);
  put_loaf(cells, 9, 0);
  put_boat(cells, 15, 0);
  put_tub(cells, 20, 0);

  // Oscillators start: 0, 10 size: 7x35
  put_pentadecathlon(cells, 0, 6);
  put_blinker(cells, 0, 15);
  put_toad(cells, 0, 20);
  put_beacon(cells, 0, 26);
  put_pulsar(cells, 6, 16);

  // Spaceships
  put_lwss(cells, 25, 0);
  put_mwss(cells, 25, 10);
  put_hwss(cells, 25, 20);
};

Simulator::Simulator(ifstream &input) {
  string error{"Simulator parsing error:\n"};
  string buffer;
  int line = 0;

  pair<int, int> window_size = get_window_size();
  // Reserving space for simulation's name and prompt
  window_size.first -= 4;
  cells = Cells(window_size);

  // #Life 1.06
  getline(input, buffer);
  line++;

  if (buffer != "#Life 1.06") {
    throw invalid_argument((stringstream()
                            << error << "Line " << line
                            << ": Input file has wrong file format")
                               .str());
  }

  // #N Simulation Name
  getline(input, buffer);
  line++;

  if (buffer.empty() || buffer.find("#N ") == string::npos) {
    throw invalid_argument((stringstream() << error << "Line " << line
                                           << ": Input file has wrong name")
                               .str());
  }
  name = buffer.substr(3, buffer.length() - 3);

  // #R B{0-8}/S{0-8}
  getline(input, buffer);
  line++;

  if (buffer.empty() || buffer.find("#R ") == string::npos ||
      buffer.find("B") == string::npos || buffer.find("S") == string::npos) {
    throw invalid_argument((stringstream() << error << "Line " << line
                                           << ": Input file has wrong rules")
                               .str());
  }

  stringstream rules(buffer.substr(3, buffer.length() - 3), ios_base::in);

  while (getline(rules, buffer, '\\')) {
    if (buffer.empty()) {
      throw invalid_argument((stringstream() << error << "Line " << line
                                             << ": Missing rule values")
                                 .str());
    }

    char rule = buffer[0];
    string allowed = "12345678";

    switch (rule) {
    case 'B':
      birth_rule = buffer.substr(1, buffer.length() - 1);
      for (const char c : birth_rule) {
        if (allowed.find(c) == string::npos) {
          throw invalid_argument((stringstream() << error << "Line " << line
                                                 << ": Wrong rule value " << c)
                                     .str());
        }
      }
      break;
    case 'S':
      survival_rule = buffer.substr(1, buffer.length() - 1);
      for (const char c : survival_rule) {
        if (allowed.find(c) == string::npos) {
          throw invalid_argument((stringstream() << error << "Line " << line
                                                 << ": Wrong rule value " << c)
                                     .str());
        }
      }
      break;
    default:
      throw invalid_argument((stringstream() << error << "Line " << line
                                             << ": Wrong rule " << rule)
                                 .str());
    }
  }

  if (birth_rule.empty() || survival_rule.empty()) {
    throw invalid_argument(
        (stringstream() << error << "Line " << line << "Missing rule values")
            .str());
  }

  // x y
  int x, y;
  string x_str, y_str;
  while (getline(input, buffer)) {
    line++;

    stringstream coords(buffer, ios_base::in);

    coords >> x;
    if (coords.fail()) {
      throw invalid_argument((stringstream()
                              << error << "Line " << line
                              << ": Wrong coordinates " << buffer)
                                 .str());
    }

    coords >> y;
    if (coords.fail()) {
      throw invalid_argument((stringstream()
                              << error << "Line " << line
                              << ": Wrong coordinates " << buffer)
                                 .str());
    }

    cells[y][x] = true;
  }
};

int Simulator::count_neighbours(int y, int x) {
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
  while (n-- > 0) {
    live();
  }
}
void Simulator::live() {
  Cells new_cells(cells.size);

  for (int y = 0; y < cells.size.first; y++) {
    for (int x = 0; x < cells.size.second; x++) {
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

int Simulator::get_height() const { return cells.size.first; }
int Simulator::get_width() const { return cells.size.second; }
const pair<int, int> Simulator::get_size() const { return cells.size; }

const string Simulator::get_name() const { return name; };
Cells &Simulator::get_cells() { return cells; }

void put_block(Cells &cells, int y, int x) {
  if (cells.size.first < 4 || cells.size.second < 4) {
    return;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 2] = true;
}

void put_beehive(Cells &cells, int y, int x) {
  if (cells.size.first < 5 || cells.size.second < 6) {
    return;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;
}

void put_loaf(Cells &cells, int y, int x) {
  if (cells.size.first < 6 || cells.size.second < 6) {
    return;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 4] = true;
  cells[y + 4][x + 3] = true;
}

void put_boat(Cells &cells, int y, int x) {
  if (cells.size.first < 5 || cells.size.second < 5) {
    return;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;
}

void put_tub(Cells &cells, int y, int x) {
  if (cells.size.first < 5 || cells.size.second < 5) {
    return;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;
}

void put_blinker(Cells &cells, int y, int x) {
  if (cells.size.first < 5 || cells.size.second < 5) {
    return;
  }

  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 2] = true;
  cells[y + 2][x + 3] = true;
}

void put_toad(Cells &cells, int y, int x) {
  if (cells.size.first < 6 || cells.size.second < 6) {
    return;
  }

  cells[y + 2][x + 2] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 1] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;
}

void put_beacon(Cells &cells, int y, int x) {
  if (cells.size.first < 6 || cells.size.second < 6) {
    return;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 2] = true;
  cells[y + 3][x + 3] = true;
  cells[y + 3][x + 4] = true;
  cells[y + 4][x + 3] = true;
  cells[y + 4][x + 4] = true;
}

void put_pulsar(Cells &cells, int y, int x) {
  if (cells.size.first < 17 || cells.size.second < 17) {
    return;
  }

  cells[y + 2][x + 4] = true;
  cells[y + 2][x + 5] = true;
  cells[y + 2][x + 6] = true;
  cells[y + 2][x + 10] = true;
  cells[y + 2][x + 11] = true;
  cells[y + 2][x + 12] = true;

  cells[y + 4][x + 2] = true;
  cells[y + 4][x + 7] = true;
  cells[y + 4][x + 9] = true;
  cells[y + 4][x + 14] = true;

  cells[y + 5][x + 2] = true;
  cells[y + 5][x + 7] = true;
  cells[y + 5][x + 9] = true;
  cells[y + 5][x + 14] = true;

  cells[y + 6][x + 2] = true;
  cells[y + 6][x + 7] = true;
  cells[y + 6][x + 9] = true;
  cells[y + 6][x + 14] = true;

  cells[y + 7][x + 4] = true;
  cells[y + 7][x + 5] = true;
  cells[y + 7][x + 6] = true;
  cells[y + 7][x + 10] = true;
  cells[y + 7][x + 11] = true;
  cells[y + 7][x + 12] = true;

  cells[y + 9][x + 4] = true;
  cells[y + 9][x + 5] = true;
  cells[y + 9][x + 6] = true;
  cells[y + 9][x + 10] = true;
  cells[y + 9][x + 11] = true;
  cells[y + 9][x + 12] = true;

  cells[y + 10][x + 2] = true;
  cells[y + 10][x + 7] = true;
  cells[y + 10][x + 9] = true;
  cells[y + 10][x + 14] = true;

  cells[y + 11][x + 2] = true;
  cells[y + 11][x + 7] = true;
  cells[y + 11][x + 9] = true;
  cells[y + 11][x + 14] = true;

  cells[y + 12][x + 2] = true;
  cells[y + 12][x + 7] = true;
  cells[y + 12][x + 9] = true;
  cells[y + 12][x + 14] = true;

  cells[y + 14][x + 4] = true;
  cells[y + 14][x + 5] = true;
  cells[y + 14][x + 6] = true;
  cells[y + 14][x + 10] = true;
  cells[y + 14][x + 11] = true;
  cells[y + 14][x + 12] = true;
}

void put_pentadecathlon(Cells &cells, int y, int x) {
  if (cells.size.first < 18 || cells.size.second < 11) {
    return;
  }

  cells[y + 4][x + 5] = true;
  cells[y + 5][x + 5] = true;
  cells[y + 6][x + 4] = true;
  cells[y + 6][x + 6] = true;
  cells[y + 7][x + 5] = true;
  cells[y + 8][x + 5] = true;
  cells[y + 9][x + 5] = true;
  cells[y + 10][x + 5] = true;
  cells[y + 11][x + 4] = true;
  cells[y + 11][x + 6] = true;
  cells[y + 12][x + 5] = true;
  cells[y + 13][x + 5] = true;
}

void put_glider(Cells &cells, int y, int x) {
  if (cells.size.first < 5 || cells.size.second < 5) {
    return;
  }

  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;
}
void put_lwss(Cells &cells, int y, int x) {
  if (cells.size.first < 6 || cells.size.second < 7) {
    return;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 4] = true;
  cells[y + 2][x + 5] = true;
  cells[y + 3][x + 1] = true;
  cells[y + 3][x + 5] = true;
  cells[y + 4][x + 2] = true;
  cells[y + 4][x + 3] = true;
  cells[y + 4][x + 4] = true;
  cells[y + 4][x + 5] = true;
}

void put_mwss(Cells &cells, int y, int x) {
  if (cells.size.first < 7 || cells.size.second < 8) {
    return;
  }

  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 5] = true;
  cells[y + 3][x + 6] = true;
  cells[y + 4][x + 1] = true;
  cells[y + 4][x + 6] = true;
  cells[y + 5][x + 2] = true;
  cells[y + 5][x + 3] = true;
  cells[y + 5][x + 4] = true;
  cells[y + 5][x + 5] = true;
  cells[y + 5][x + 6] = true;
}

void put_hwss(Cells &cells, int y, int x) {
  if (cells.size.first < 7 || cells.size.second < 9) {
    return;
  }

  cells[y + 1][x + 3] = true;
  cells[y + 1][x + 4] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 6] = true;
  cells[y + 3][x + 7] = true;
  cells[y + 4][x + 1] = true;
  cells[y + 4][x + 7] = true;
  cells[y + 5][x + 2] = true;
  cells[y + 5][x + 3] = true;
  cells[y + 5][x + 4] = true;
  cells[y + 5][x + 5] = true;
  cells[y + 5][x + 6] = true;
  cells[y + 5][x + 7] = true;
}
