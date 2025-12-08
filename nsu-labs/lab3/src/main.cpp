#include "parser.h"
#include "soundp.h"
#include <iostream>

int main(int argc, const char **argv) {
  vector<string> wav_files;
  string config_file;
  unique_ptr<SoundProcessor> soundp;

  ArgParser parser(argc, argv);
  try {
    parser.parse();
  } catch (const exception &e) {
    cerr << e.what();
    return 1;
  }
  parser.get(config_file, wav_files);

  if (config_file.empty() || wav_files.size() == 0) {
    return 0;
  }

  try {
    soundp = make_unique<SoundProcessor>(config_file, wav_files);
  } catch (const exception &e) {
    cerr << "Failed to create SoundProcessor:\n" << e.what();
    return 1;
  }

  try {
    soundp->process();
  } catch (const exception &e) {
    cerr << "Failed to process audio files:\n" << e.what();
    return 1;
  }

  return 0;
}
