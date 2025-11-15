#include "renderer.h"
#include "simulator.h"

struct NullDeleter {
  void operator()(std::ostream *) const noexcept {}
};

Renderer::Renderer(Cells &cells, ostream &output)
    : cells(cells), canvas(output) {}

Renderer::Renderer(Cells &cells, ofstream &output)
    : cells(cells), canvas(output) {}

void Renderer::render(std::string const &name) { render(canvas, name); }

void Renderer::render(std::ostream &output, const std::string &name) {
  output << name << endl;
  for (const auto &row : cells) {
    for (const auto &cell : row) {
      output << graphics[cell];
    }
    output << endl;
  }
}

void Renderer::clean() { system(CLEAR_COMMAND); }
