#include "controller.h"
#include "simulator.h"
#include <chrono>
#include <stdexcept>
#include <thread>

void Controller::help() {
  cout << "Game of life interactive mode" << endl
       << "Commands:" << endl
       << "  help            - show this mesaage" << endl
       << "  tick <n=1>      - live n iterations" << endl
       << "  dump <filename> - write current status to file" << endl
       << "  load <filename> - load status from file" << endl
       << "  clear           - clear cells" << endl
       << "  anim            - toggle animations" << endl
       << "  quit            - finish the game" << endl;
}

Controller::Controller(Simulator &sim, Render &ren) : sim(sim), ren(ren) {};

bool Controller::handle_tick(string ticks) {
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

  if (anim) {
    for (int i = 0; i < n; i++) {
      sim.live();
      ren.clean();
      ren.render(sim.get_name());
      this_thread::sleep_for(chrono::milliseconds(300));
    }
  } else {
    sim.live(n);
  }

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

  sim << output;
  statusline = "Dumped to " + file;

  output.close();
  return true;
}

bool Controller::handle_load(string &file) {
  if (file.empty()) {
    statusline = "No file given";
    return false;
  }

  ifstream input(file);

  if (input.fail() || !input.is_open()) {
    statusline = "Failed to open file " + file;
    return false;
  }

  try {
    sim >> input;
  } catch (const exception &e) {
    statusline = "Failed to load from " + file + ": " + e.what();
    return false;
  }

  statusline = "Loaded from " + file;

  input.close();
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
  } else if (command == "load") {
    handle_load(value);
  } else if (command == "clear") {
    sim.get_cells().clear();
    statusline = "Cells field was cleared";
  } else if (command == "anim") {
    anim = !anim;
    statusline = anim ? "Enabled animations" : "Disabled animations";
  } else if (command == "quit" || command == "exit") {
    return false;
  } else if (input.empty()) {
    statusline = commands;
    return true;
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
    ren.render(cout, sim.get_name());

    cout << statusline << endl;
    cout << prompt;

    getline(cin, input);

    running = handle_input(input);
  }
}
