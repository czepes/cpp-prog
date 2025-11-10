#include "parser.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  ArgParser parser(argc, argv);

  string infile;
  string outfile;
  int iterations;

  parser.parse();
  parser.get(&infile, &outfile, &iterations);

  cout << "Input file:  " << infile << endl
       << "Output file: " << outfile << endl
       << "Iterations:  " << iterations << endl;

  return 0;
}
