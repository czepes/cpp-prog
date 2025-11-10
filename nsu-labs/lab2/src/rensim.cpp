#include "renderer.h"
#include "simulator.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;

int main(int argc, char **argv) {
  int iterations = 100;
  Simulator sim;
  Renderer ren(sim.get_size());

  ren.render(sim.get_cells(), sim.get_name());
  sleep_for(nanoseconds(200000000));
  while (iterations--) {
    sim.live();
    cout << "\033[35A";
    ren.render(sim.get_cells(), sim.get_name());
    sleep_for(nanoseconds(200000000));
  }

  return EXIT_SUCCESS;
}
