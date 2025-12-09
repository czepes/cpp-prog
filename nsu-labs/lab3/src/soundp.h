#ifndef SOUNDP_H
#define SOUNDP_H

#include "config-parser.h"
#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class SoundProcessor {
private:
  ConfigParser parser;
  const vector<string> &files;

public:
  SoundProcessor(const string &config_file, const vector<string> &files)
      : parser(config_file, files), files(files) {
    if (files.size() < 2) {
      throw runtime_error(
          "Expected at least 2 files: output.wav and input.wav");
    }
  };
  ~SoundProcessor();
  void process();

private:
  const string tmp1 = generate_temp_filename("tmp1", ".wav");
  const string tmp2 = generate_temp_filename("tmp2", ".wav");

  string static generate_temp_filename(const string &prefix,
                                       const string &postfix) {
    static int counter{0};
    auto now = chrono::system_clock::now();
    auto timestamp =
        chrono::duration_cast<chrono::microseconds>(now.time_since_epoch())
            .count();
    return prefix + "_" + to_string(timestamp) + "_" + to_string(counter++) +
           postfix;
  }
};

#endif
