#ifndef MIXER_H
#define MIXER_H

#include "../converter.h"
#include <memory>

class Mixer : public Converter {
private:
  shared_ptr<WavReader> input2;
  double start_time;
  double end_time;

public:
  Mixer(shared_ptr<WavWriter> output, shared_ptr<WavReader> input,
        shared_ptr<WavReader> input2, double start_time = 0,
        double end_time = -1);

  static unique_ptr<Mixer> create(shared_ptr<WavWriter> output,
                                  shared_ptr<WavReader> input,
                                  const vector<string> &params,
                                  int line_num = 0);

  void convert() override;

  static string get_name() { return "Mixer"; }
  static string get_desc() {
    return "Mix two audio files in specified time interval";
  }
  static string get_command() { return "mix"; }
  static string get_usage() { return get_command() + " [$n] [start] [end]"; };
};

#endif
