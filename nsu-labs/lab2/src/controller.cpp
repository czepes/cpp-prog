#include "controller.h"
#include "simulator.h"
#include <stdexcept>

void Controller::help() {
  cout << "Game of life interactive mode" << endl
       << "Commands:" << endl
       << "  help            - show this mesaage" << endl
       << "  tick <n=1>      - live n iterations" << endl
       << "  dump <filename> - write current status to file" << endl
       << "  quit            - finish the game" << endl;
}

Controller::Controller(Simulator &sim, Renderer &ren) : sim(sim), ren(ren) {};

bool Controller::handle_tick(string &ticks) {
  int n{1};

  if (ticks.empty()) {
    sim.live(n);
    statusline = "Lived for " + to_string(n) + " iteration(s)";
    return true;
  }

  try {
    n = stoi(ticks);
  } catch (const invalid_argument &e) {
    statusline = "Wrong value " + ticks;
    return false;
  } catch (const out_of_range &e) {
    statusline = "Value " + ticks + " is too large";
    return false;
  }

  if (n < 0) {
    statusline = "Value is negative";
    return false;
  }

  sim.live(n);
  statusline = "Lived for " + to_string(n) + " iteration(s)";

  return true;
}

bool Controller::handle_dump(string &file) {
  if (file.empty()) {
    statusline = "No file given";
    return false;
  }

  ofstream output(file);

  if (output.fail() || !output.is_open()) {
    statusline = "Failed to open file " + file;
    return false;
  }

  ren.render(output, sim.get_cells(), sim.get_name());
  statusline = "Dumped to " + file;

  output.close();
  return true;
}

bool Controller::handle_input(string &input) {
  string command;
  string value;
  stringstream ss(input);
  ss >> command;
  ss >> value;

  if (command == "help") {
    ren.clean();
    help();
    cout << prompt;
    getline(cin, input);
    statusline = commands;
  } else if (command == "tick") {
    handle_tick(value);
  } else if (command == "dump") {
    handle_dump(value);
  } else if (command == "quit" || command == "exit") {
    return false;
  } else {
    statusline = "Unknown command: " + input;
  }

  return true;
}

void Controller::start() {
  string input;
  bool running{true};

  statusline = commands;

  while (running) {
    ren.clean();
    ren.render(cout, sim.get_cells(), sim.get_name());

    cout << statusline << endl;
    cout << prompt;
    getline(cin, input);

    running = handle_input(input);
  }
}
