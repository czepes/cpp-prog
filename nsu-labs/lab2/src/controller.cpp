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

void Controller::clean() {
  // cout << "\033[2J\033[H";
  // cout << "\033[38A";
  system("clear");
}

bool Controller::handle_tick(string &input, int *n, string &statusline) {

  stringstream ss(input);
  string buf;

  ss >> buf;

  if (buf != "tick") {
    statusline = "Unknown command " + input;
    return false;
  }

  if (ss.eof()) {
    statusline = "Missing value";
    return false;
  }

  ss >> buf;

  // TODO: handle tick<space>
  if (buf.empty() || buf == "tick") {
    statusline = "Missing value";
    return false;
  }

  try {
    *n = stoi(buf);
  } catch (const invalid_argument &e) {
    statusline = "Wrong value " + buf;
    return false;
  } catch (const out_of_range &e) {
    statusline = "Value " + buf + " is too large";
    return false;
  }

  if ((*n) < 0) {
    statusline = "Value is negative";
    return false;
  }

  return true;
}

bool Controller::handle_dump(string &input, string &statusline) {
  stringstream ss(input);
  string buf;

  ss >> buf;

  if (buf != "dump") {
    statusline = "Unknown command " + input;
    return false;
  }

  // TODO: handle dump<space>
  if (ss.eof()) {
    statusline = "Missing filename";
    return false;
  }

  ss >> buf;
  ofstream output(buf);

  if (output.fail() || !output.is_open()) {
    statusline = "Failed to open file " + buf;
    return false;
  }

  ren.render(output, sim.get_cells(), sim.get_name());
  statusline = "Dumped to " + buf;

  output.close();
  return true;
}

void Controller::start() {
  // TODO: interactive mode:
  //  1. read user's input commands
  //  2. handle these commands
  //  3. handle wrong input
  //  4. Options:
  //    [x] tick <n=1>      - live <n> iteration
  //    [x] dump <filename> - save to <filename>
  //    [o] quit
  //    [o] help
  string input;
  string prompt{"$ "};
  string commands{"help | tick <n=1> | dump <filename> | quit"};
  string statusline{commands};

  while (true) {
    clean();
    ren.render(cout, sim.get_cells(), sim.get_name());

    cout << statusline << endl;
    cout << prompt;
    getline(cin, input);

    if (input == "help") {
      clean();
      help();
      cout << prompt;
      getline(cin, input);
      statusline = commands;
    } else if (input.find("tick") == 0) {
      int n{1};

      if (input != "tick" && !handle_tick(input, &n, statusline)) {
        continue;
      }

      sim.live(n);
      statusline = "Lived for " + to_string(n) + " iteration(s)";
    } else if (input.find("dump") == 0) {
      handle_dump(input, statusline);
    } else if (input == "quit" || input == "exit") {
      break;
    } else if (input.empty()) {
      continue;
    } else {
      statusline = "Unknown command: " + input;
    }
  }
}
