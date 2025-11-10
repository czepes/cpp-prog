#ifndef PARSER
#define PARSER

#include <string>

using namespace std;

class ArgParser {
private:
  int argc;
  char **argv;
  string outfile{""};
  string infile{""};
  int iterations{0};

public:
  ArgParser(int argc, char **argv) : argc(argc), argv(argv) {};

  const ArgParser &parse();
  const ArgParser &get(string *infile, string *outfile, int *i) const;
  static void help();
};

#endif
