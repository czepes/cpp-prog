#ifndef CONTROLLER
#define CONTROLLER

#include "renderer.h"
#include "simulator.h"

using namespace std;

class Controller {
private:
  Renderer &ren;
  Simulator &sim;

  bool handle_dump(string &input, string &statusline);
  bool handle_tick(string &input, int *n, string &statusline);
  static void help();

public:
  Controller(Simulator &sim, Renderer &ren);
  void start();
  void clean();
};

#endif
