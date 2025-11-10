#ifndef CONTROLLER
#define CONTROLLER

#include "renderer.h"
#include "simulator.h"

using namespace std;

class Controller {
private:
  const string prompt{"$ "};
  const string commands{"help | tick <n=1> | dump <filename> | quit"};

  Renderer &ren;
  Simulator &sim;
  string statusline;

  bool handle_dump(string &file);
  bool handle_tick(string &ticks);
  bool handle_input(string &input);

  static void help();

public:
  Controller(Simulator &sim, Renderer &ren);
  void start();
};

#endif
