#include "parser.h"
#include "converter.h"
#include "errors.h"
#include <iostream>

string ArgParser::help() {
  return "Sound processor\n"
         "/path/to/prog ❯ ./soundp\n"
         "    [-c config.txt output.wav input.wav [input1.wav, ...]]\n"
         "    [-h | --help]\n" +
         ConverterFactory::help();
}

ArgParser &ArgParser::parse() {
  if (argc <= 1) {
    throw ArgParserError("Missing arguments:\n" + help());
  }

  for (int i = 1; i < argc; i++) {
    const string arg(argv[i]);

    if (arg == "--help" || arg == "-h") {
      cout << help();
      continue;
    }

    if (arg == "-c") {
      if (i + 3 > argc) {
        throw ArgParserError("Missing configuration values:\n" + help());
      }

      config = argv[i + 1];

      for (int j = i + 2; j < argc; j++) {
        files.push_back(argv[j]);
      }

      break;
    }

    throw ArgParserError("Unknown argument " + arg);
  }

  return *this;
}

ArgParser &ArgParser::get(string &config_file, vector<string> &files) {
  config_file = this->config;
  files = this->files;
  return *this;
}
