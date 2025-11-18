#include "controller.h"
#include "parser.h"
#include "render.h"
#include "simulator.h"

int main(int argc, char **argv) {
  ArgParser parser(argc, argv);

  int iterations;
  string infile;
  string outfile;
  ifstream input;
  ofstream output;

  try {
    parser.parse();
  } catch (const exception &e) {
    cerr << e.what();
    return 1;
  }
  parser.get(&infile, &outfile, &iterations);

  if (!infile.empty()) {
    input = ifstream(infile);

    if (!input.is_open()) {
      cerr << "Failed to open file " << infile << endl;
      if (output.is_open()) {
        output.close();
      }
      return 1;
    }
  }

  if (!outfile.empty()) {
    output = ofstream(outfile);

    if (!output.is_open()) {
      cerr << "Failed to open file " << outfile << endl;
      if (input.is_open()) {
        input.close();
      }
      return 1;
    }
  }

  unique_ptr<Simulator> sim;
  unique_ptr<Render> ren;

  if (input.is_open()) {
    try {
      sim = make_unique<Simulator>(input);
    } catch (const exception &e) {
      cerr << e.what() << endl;

      input.close();
      if (output.is_open()) {
        output.close();
      }

      return 1;
    }
  } else {
    sim = make_unique<Simulator>(true);
  }

  if (output.is_open()) {
    ren = make_unique<Render>(sim->get_cells(), output);
  } else {
    ren = make_unique<Render>(sim->get_cells());
  }

  if (iterations == 0 && outfile.empty()) {
    sim->live(iterations);
    Controller ctr(*sim, *ren);
    ctr.start();
  } else {
    sim->live(iterations);
    ren->render(sim->get_name());
  }

  if (input.is_open()) {
    input.close();
  }
  if (output.is_open()) {
    output.close();
  }
  return 0;
}
