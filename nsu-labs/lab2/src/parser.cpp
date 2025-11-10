#include "parser.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

void ArgParser::help() {
  std::cout << "Game of life program" << std::endl
            << "/path/to/prog$ program" << std::endl
            << "    [-i infile | --input=file]" << std::endl
            << "    [-o outfile | --output=file]" << std::endl
            << "    [-n int | --iterations=int]" << std::endl
            << "    [-h | --help]" << std::endl;
}

const ArgParser &ArgParser::parse() {
  for (int i = 1; i < argc; i++) {
    string arg(argv[i]);

    auto what{(stringstream() << "Missing value for " << arg)};
    string missing_value =
        (stringstream() << "Missing value for " << arg).str();
    string unknown_argument =
        (stringstream() << "Unknown agurment " << arg).str();

    if (arg.find("--") != string::npos) {
      if (arg == "--help") {
        help();
        continue;
      }

      size_t delim_pos = arg.find('=');
      string option = arg.substr(2, delim_pos - 2);
      string value = arg.substr(delim_pos + 1, arg.length() - delim_pos);

      if (option == "iterations") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          cerr << missing_value << endl;
          throw invalid_argument(missing_value);
        }
        try {
          iterations = stoi(value);
        } catch (std::invalid_argument e) {
          cerr << "Could not convert " << value << " to int" << endl;
          throw e;
        } catch (std::out_of_range e) {
          cerr << "Value " << value << " is too big" << endl;
          throw e;
        }
      } else if (option == "input") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          cerr << missing_value << endl;
          throw invalid_argument(missing_value);
        }
        infile = value;
      } else if (option == "output") {
        if (delim_pos == string::npos || delim_pos == arg.length() - 1) {
          cerr << missing_value << endl;
          throw invalid_argument(missing_value);
        }
        outfile = value;
      } else {
        cerr << unknown_argument << endl;
        throw invalid_argument(unknown_argument);
      }
    } else if (arg.find("-") != string::npos) {

      if (arg.length() != 2) {
        cerr << unknown_argument << endl;
        throw invalid_argument(unknown_argument);
      }

      if (arg == "-h") {
        help();
        continue;
      }

      if (i == argc - 1) {
        cerr << missing_value << endl;
        throw invalid_argument(missing_value);
      }

      string value = argv[++i];

      if (arg == "-n") {
        try {
          iterations = stoi(value);
        } catch (invalid_argument e) {
          cerr << "Could not convert " << value << " to int" << endl;
          throw e;
        } catch (out_of_range e) {
          cerr << "Value " << value << " is too big" << endl;
          throw e;
        }
      } else if (arg == "-i") {
        infile = value;
      } else if (arg == "-o") {
        outfile = value;
      } else {
        cerr << unknown_argument << endl;
        throw invalid_argument(unknown_argument);
      }
    } else {
      cerr << unknown_argument << endl;
      throw invalid_argument(unknown_argument);
    }
  }

  return *this;
}

const ArgParser &ArgParser::get(std::string *infile, std::string *outfile,
                                int *i) const {
  *infile = this->infile;
  *outfile = this->outfile;
  *i = this->iterations;

  return *this;
}
