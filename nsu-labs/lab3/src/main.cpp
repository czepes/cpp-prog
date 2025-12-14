#include "converter.h"
#include "errors.h"
#include "parser.h"
#include "soundp.h"
#include <cstdlib>
#include <iostream>
#include <memory>

int main(int argc, const char **argv) {
  ConverterFactory::init();

  vector<string> files;
  string config_file;
  unique_ptr<SoundProcessor> soundp;

  ArgParser parser(argc, argv);
  try {
    parser.parse().get(config_file, files);
    soundp = make_unique<SoundProcessor>(config_file, files);
    soundp->process();
  } catch (const SoundError &e) {
    cerr << e.what();
    return e.code();
  }

  return EXIT_SUCCESS;
}
