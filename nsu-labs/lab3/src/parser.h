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
  string input;
  vector<string> files;

  static string help();

public:
  ArgParser(int argc, const char **argv) : argc(argc), argv(argv) {};
  void parse();
  void get(string &config, vector<string> &files) const;
};

#endif
