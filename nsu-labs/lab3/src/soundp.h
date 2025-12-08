#ifndef SOUNDP
#define SOUNDP

#include "wav.h"
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class Converter {
protected:
  shared_ptr<WavWriter> dest;
  shared_ptr<WavReader> src;

public:
  Converter(shared_ptr<WavReader> src, shared_ptr<WavWriter> dest)
      : dest(dest), src(src) {};
  virtual ~Converter() = default;
  virtual void convert() = 0;
  static unique_ptr<Converter> fabricate(shared_ptr<WavReader> src,
                                         shared_ptr<WavWriter> dest);
  static string info();
};

class Muter : public Converter {
private:
  const int start;
  const int end;

public:
  Muter(shared_ptr<WavReader> src, shared_ptr<WavWriter> dest, int start = 0,
        int end = -1)
      : Converter(src, dest), start(start), end(end) {};

  static unique_ptr<Muter> fabricate(shared_ptr<WavReader> src,
                                     shared_ptr<WavWriter> dest, int start = 0,
                                     int end = -1) {
    return make_unique<Muter>(src, dest, start, end);
  };

  void convert() override;
  static string info() { return "Mute audio file: mute [start] [end]\n"; };
};

class Mixer : public Converter {
private:
  unique_ptr<WavReader> src2;
  const int start;
  const int end;

public:
  Mixer(shared_ptr<WavReader> src1, unique_ptr<WavReader> src2,
        shared_ptr<WavWriter> dest, int start = 0, int end = -1)
      : Converter(src1, dest), src2(std::move(src2)), start(start), end(end) {};

  static unique_ptr<Mixer> fabricate(shared_ptr<WavReader> src1,
                                     unique_ptr<WavReader> src2,
                                     shared_ptr<WavWriter> dest, int start = 0,
                                     int end = -1) {
    return make_unique<Mixer>(src1, std::move(src2), dest, start, end);
  }

  void convert() override;
  static string info() {
    return "Mix 2 audio files: mix [$n] [start] [end]\n";
  };
};

class Cropper : public Converter {
private:
  const int start;
  const int end;

public:
  Cropper(shared_ptr<WavReader> src, shared_ptr<WavWriter> dest, int start = 0,
          int end = -1)
      : Converter(src, dest), start(start), end(end) {};

  static unique_ptr<Cropper> fabricate(shared_ptr<WavReader> src,
                                       shared_ptr<WavWriter> dest,
                                       int start = 0, int end = -1) {
    return make_unique<Cropper>(src, dest, start, end);
  }

  void convert() override;
  static string info() { return "Reverse audio file: crop [start] [end]\n"; }
};

class Gainer : public Converter {
private:
  const double factor;
  const int start;
  const int end;

public:
  Gainer(shared_ptr<WavReader> src, shared_ptr<WavWriter> dest, double factor,
         int start = 0, int end = -1)
      : Converter(src, dest), factor(factor), start(start), end(end) {};

  static unique_ptr<Gainer> fabricate(shared_ptr<WavReader> src,
                                      shared_ptr<WavWriter> dest, double factor,
                                      int start = 0, int end = -1) {
    return make_unique<Gainer>(src, dest, factor, start, end);
  }

  void convert() override;
  static string info() {
    return "In/Decrease volume: gain [factor] [start] [end]\n";
  }
};

static vector<string (*)()> converters_info = {Muter::info, Mixer::info,
                                               Cropper::info, Gainer::info};

class ConfigParser {
private:
  unique_ptr<ifstream> input;
  const vector<string> &files;
  int line_idx = 0;
  bool owns;

  void parse_range(stringstream &ss, int &start, int &end, const double maxv);
  void parse_file(stringstream &ss, int &file_idx);
  void parse_factor(stringstream &ss, double &factor, const double minv = 0,
                    const double maxv = -1);

public:
  ConfigParser(ifstream &input, const vector<string> &files)
      : input(&input), files(files), owns(false) {};

  ConfigParser(const string &config_file, const vector<string> &files)
      : input(make_unique<ifstream>(config_file)), files(files), owns(true) {
    if (!*input) {
      throw runtime_error("Failed to open config file: " + config_file);
    }
  };

  ~ConfigParser() {
    if (owns && input->is_open()) {
      input->close();
    }
  }

  int get_line_idx() { return line_idx; }

  unique_ptr<Converter> parse(shared_ptr<WavReader> in,
                              shared_ptr<WavWriter> out);
  bool eof() { return input->eof(); };
};

class SoundProcessor {
private:
  ConfigParser parser;
  const vector<string> &files;

public:
  SoundProcessor(ifstream &input, const vector<string> &wav_files)
      : parser(input, wav_files), files(wav_files) {
    if (files.size() < 2) {
      throw runtime_error(
          "Expected at least 2 files: output.wav and input.wav");
    }
  };
  SoundProcessor(const string &config_file, const vector<string> &wav_files)
      : parser(config_file, wav_files), files(wav_files) {
    if (files.size() < 2) {
      throw runtime_error(
          "Expected at least 2 files: output.wav and input.wav");
    }
  };
  ~SoundProcessor();
  void process();

private:
  const string tmp1 = unique_name("tmp1", ".wav");
  const string tmp2 = unique_name("tmp2", ".wav");

  string static unique_name(const string &prefix, const string &postfix) {
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
