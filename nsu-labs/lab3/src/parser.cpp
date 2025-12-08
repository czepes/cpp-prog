#include "parser.h"
#include "soundp.h"
#include <iostream>

string ArgParser::help() {
  string message = "Sound processor\n"
                   "/path/to/prog ❯ ./soundp\n"
                   "    [-c config.txt input1.wav [input2.wav, ...]]\n"
                   "    [-h | --help]\n"
                   "Configuration file options:\n";
  for (auto info : converters_info) {
    message += info();
  }
  return message;
}

void ArgParser::parse() {
  if (argc <= 1) {
    throw runtime_error("Missing arguments:\n" + help());
  }
  for (int i = 1; i < argc; i++) {
    const string arg(argv[i]);
    const string missing_value{"Missing value for " + arg};
    const string unknown_argument{"Unknown argument " + arg};

    if (arg == "--help" || arg == "-h") {
      cout << help();
      continue;
    }

    if (arg == "-c") {
      if (i + 2 >= argc) {
        throw invalid_argument(missing_value);
      }

      config = argv[i + 1];

      for (int j = i + 2; j < argc; j++) {
        files.push_back(argv[j]);
      }

      break;
    }

    throw invalid_argument(unknown_argument);
  }
}

void ArgParser::get(string &config, vector<string> &files) const {
  config = this->config;
  files = this->files;
}
