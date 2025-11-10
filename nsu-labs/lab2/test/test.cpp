#include "../src/renderer.h"

#include <iostream>
#include <set>

int main() {
  int width = 160;
  int height = 41;

  Renderer renderer(width, height, &std::cout);

  std::vector<std::set<int>> cells(height);

  for (int i = 0; i < height; i++) {
    cells[i] = {i * 4, i * 4 + 1, width - i * 4 - 2, width - i * 4 - 1};
  }

  renderer.render(cells);

  return 0;
}
