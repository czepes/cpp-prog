#include "patterns.h"

bool put_cell(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first == 0 || size.second == 0) {
    return false;
  }

  cells[y][x] = true;

  return true;
}

bool put_block(Cells &cells, int y, int x, int height, int width, bool fill) {
  pair<int, int> size = cells.get_size();
  if (size.first < height || size.second < width) {
    return false;
  }

  for (int i = y; i < y + height; i++) {
    for (int j = x; j < x + width; j++) {
      cells[i][j] = fill || i == y || i == (y + height - 1) || j == x ||
                    j == (x + width - 1);
    }
  }

  return true;
}

bool find_block(Cells &cells, int y, int x, int height, int width, bool fill) {
  pair<int, int> size = cells.get_size();
  if (size.first < height || size.second < width) {
    return false;
  }

  for (int i = y; i < y + height; i++) {
    for (int j = x; j < x + width; j++) {
      if ((fill || i == y || (i == y + height - 1) || j == x ||
           (j == x + width - 1)) &&
          !cells[i][j]) {
        return false;
      }
    }
  }

  return true;
}

bool put_beehive(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 5 || size.second < 6) {
    return false;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;

  return true;
}

bool put_loaf(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 6 || size.second < 6) {
    return false;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 4] = true;
  cells[y + 4][x + 3] = true;

  return true;
}

bool put_boat(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 5 || size.second < 5) {
    return false;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;

  return true;
}

bool put_tub(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 5 || size.second < 5) {
    return false;
  }

  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;

  return true;
}

bool put_blinker(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 5 || size.second < 5) {
    return false;
  }

  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 2] = true;
  cells[y + 2][x + 3] = true;

  return true;
}

bool put_toad(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 6 || size.second < 6) {
    return false;
  }

  cells[y + 2][x + 2] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 2][x + 4] = true;
  cells[y + 3][x + 1] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;

  return true;
}

bool put_beacon(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 6 || size.second < 6) {
    return false;
  }

  cells[y + 1][x + 1] = true;
  cells[y + 1][x + 2] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 2] = true;
  cells[y + 3][x + 3] = true;
  cells[y + 3][x + 4] = true;
  cells[y + 4][x + 3] = true;
  cells[y + 4][x + 4] = true;

  return true;
}

bool put_pulsar(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 17 || size.second < 17) {
    return false;
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

  return true;
}

bool put_pentadecathlon(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 18 || size.second < 11) {
    return false;
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

  return true;
}

bool put_glider(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 5 || size.second < 5) {
    return false;
  }

  cells[y + 1][x + 3] = true;
  cells[y + 2][x + 1] = true;
  cells[y + 2][x + 3] = true;
  cells[y + 3][x + 2] = true;
  cells[y + 3][x + 3] = true;

  return true;
}

bool put_lwss(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 6 || size.second < 7) {
    return false;
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

  return true;
}

bool put_mwss(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 7 || size.second < 8) {
    return false;
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

  return true;
}

bool put_hwss(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 7 || size.second < 9) {
    return false;
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

  return true;
}

bool put_glider_gun(Cells &cells, int y, int x) {
  pair<int, int> size = cells.get_size();
  if (size.first < 11 || size.second < 38) {
    return false;
  }

  put_block(cells, 5, 1);
  put_block(cells, 3, 35);

  put_block(cells, 3, 13, 1, 2);
  cells[4][12] = true;
  cells[4][16] = true;
  put_block(cells, 5, 11, 3, 1);
  cells[6][15] = true;
  put_block(cells, 5, 17, 3, 1);
  cells[6][18] = true;
  cells[8][12] = true;
  cells[8][16] = true;
  put_block(cells, 9, 13, 1, 2);

  put_block(cells, 3, 21, 3, 2);
  cells[2][23] = true;
  cells[6][23] = true;
  put_block(cells, 1, 25, 2, 1);
  put_block(cells, 6, 25, 2, 1);

  return true;
}
