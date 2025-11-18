#ifndef CONTROLLER
#define CONTROLLER

#include "render.h"
#include "simulator.h"
#include <chrono>

using namespace std;

class Controller {
private:
  const string prompt{"$ "};
  const string commands{
      "help | tick <n=1> | dump <filename> | load <filename> | quit"};

  Render &ren;
  Simulator &sim;
  string statusline;
  bool anim{true};

  bool handle_dump(string &file);
  bool handle_load(string &file);
  bool handle_tick(string ticks = "");
  bool handle_input(string &input);

  static void help();

public:
  Controller(Simulator &sim, Render &ren);
  void start();
};

#endif
