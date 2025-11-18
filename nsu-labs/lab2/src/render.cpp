#include "render.h"

Render::Render(Cells &cells, ostream &output) : cells(cells), canvas(output) {}
Render::Render(Cells &cells, ofstream &output) : cells(cells), canvas(output) {}

void Render::render(const string &name) { render(cells, name, canvas); }
void Render::render(ostream &output, const string &name) {
  render(cells, name, output);
}

void Render::clean() { system(CLEAR_COMMAND); }
