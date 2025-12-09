#include "converter.h"
#include "parser.h"
#include "soundp.h"
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
  } catch (const exception &e) {
    cerr << e.what();
    return 1;
  }

  if (config_file.empty() || files.size() == 0) {
    return 0;
  }

  try {
    soundp = make_unique<SoundProcessor>(config_file, files);
    soundp->process();
  } catch (const exception &e) {
    cerr << e.what();
    return 1;
  }

  return 0;
}
