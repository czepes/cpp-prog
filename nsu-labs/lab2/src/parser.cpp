#include "parser.h"
#include <iostream>
#include <stdexcept>

void ArgParser::help() {
  cout << "Game of life program" << endl
       << "/path/to/prog$ program" << endl
       << "    [-i infile | --input=file]" << endl
       << "    [-o outfile | --output=file]" << endl
       << "    [-n int | --iterations=int]" << endl
       << "    [-h | --help]" << endl;
}

const ArgParser &ArgParser::parse() {
  for (int i = 1; i < argc; i++) {
    string arg(argv[i]);
    string missing_value{"Missing value for " + arg};
    string unknown_argument{"Unknown argument " + arg};

    if (arg.find("--") == 0) {
      if (arg == "--help") {
        help();
        continue;
      }

      size_t delim_pos = arg.find('=');
      string option = arg.substr(2, delim_pos - 2);
      string value = arg.substr(delim_pos + 1, arg.length() - delim_pos);

      if (option == "iterations") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          throw invalid_argument(missing_value);
        }
        try {
          iterations = stoi(value);
        } catch (invalid_argument e) {
          throw invalid_argument("Could not convert " + value + " to int");
        } catch (out_of_range e) {
          throw out_of_range("Value " + value + " is too big");
        }
      } else if (option == "input") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          throw invalid_argument(missing_value);
        }
        infile = value;
      } else if (option == "output") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          throw invalid_argument(missing_value);
        }
        outfile = value;
      } else {
        throw invalid_argument(unknown_argument);
      }
    } else if (arg.find("-") != string::npos) {

      if (arg.length() != 2) {
        throw invalid_argument(unknown_argument);
      }

      if (arg == "-h") {
        help();
        continue;
      }

      if (i == argc - 1) {
        throw invalid_argument(missing_value);
      }

      string value = argv[++i];

      if (arg == "-n") {
        try {
          iterations = stoi(value);
        } catch (invalid_argument e) {
          throw invalid_argument("Could not convert " + value + " to int");
        } catch (out_of_range e) {
          throw out_of_range("Value " + value + " is too big");
        }
        if (iterations < 0) {
          throw out_of_range("Value " + value + " is negative");
        }
      } else if (arg == "-i") {
        infile = value;
      } else if (arg == "-o") {
        outfile = value;
      } else {
        throw invalid_argument(unknown_argument);
      }
    } else {
      throw invalid_argument(unknown_argument);
    }
  }

  return *this;
}

const ArgParser &ArgParser::get(string *infile, string *outfile, int *i) const {
  *infile = this->infile;
  *outfile = this->outfile;
  *i = this->iterations;

  return *this;
}
