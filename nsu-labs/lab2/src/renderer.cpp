#include "renderer.h"
#include "simulator.h"

struct NullDeleter {
  void operator()(std::ostream *) const noexcept {}
};

Renderer::Renderer(const pair<int, int> size, ostream &output)
    : size(size), canvas(output) {}

Renderer::Renderer(const pair<int, int> size, ofstream &output)
    : size(size), canvas(output) {}

void Renderer::render(Cells &cells, std::string const &name) {
  render(canvas, cells, name);
}

void Renderer::render(std::ostream &output, Cells &cells,
                      const std::string &name) {
  output << name << endl;
  for (int y = 0; y < cells.size.first; y++) {
    for (int x = 0; x < cells.size.second; x++) {
      output << graphics[(bool)cells[y][x]];
    }
    output << endl;
  }
}

void Renderer::clean() {
  // cout << "\033[2J\033[H";
  // cout << "\033[38A";
  system("clear");
}
