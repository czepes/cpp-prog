#ifndef SOUNDP_H
#define SOUNDP_H

#include "config-parser.h"
#include <chrono>
#include <string>
#include <vector>

using namespace std;

class SoundProcessor {
private:
  ConfigParser parser;
  const vector<string> &files;

public:
  SoundProcessor(const string &config_file, const vector<string> &files);
  ~SoundProcessor();
  void process();

private:
  string tmp1;
  string tmp2;

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
