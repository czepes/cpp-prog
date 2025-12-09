#ifndef SOUNDP_PARSER
#define SOUNDP_PARSER

#include <string>
#include <vector>

using namespace std;

class ArgParser {
private:
  const int argc;
  const char **argv;
  string config;
  vector<string> files;

  static string help();

public:
  ArgParser(int argc, const char **argv) : argc(argc), argv(argv) {};
  ArgParser &parse();
  ArgParser &get(string &config_file, vector<string> &files);
};

#endif
