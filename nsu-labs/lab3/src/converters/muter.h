#ifndef MUTER_H
#define MUTER_H

#include "../converter.h"

class Muter : public Converter {
private:
  double start_time;
  double end_time;

public:
  Muter(shared_ptr<WavReader> input, shared_ptr<WavWriter> output,
        double start_time = 0, double end_time = -1);

  static unique_ptr<Muter> create(shared_ptr<WavReader> input,
                                  shared_ptr<WavWriter> output,
                                  const vector<string> &params,
                                  int line_num = 0);

  void convert() override;

  static string get_name() { return "Muter"; }
  static string get_desc() { return "Mutes audio in specified time interval"; }
  static string get_command() { return "mute"; }
  static string get_usage() { return get_command() + " [start] [end]"; };
};

#endif
