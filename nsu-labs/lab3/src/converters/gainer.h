#ifndef GAINER_H
#define GAINER_H

#include "../converter.h"

class Gainer : public Converter {
private:
  double factor;
  double start_time;
  double end_time;

public:
  Gainer(shared_ptr<WavReader> input, shared_ptr<WavWriter> output,
         double factor = 1, double start_time = 0, double end_time = -1);

  static unique_ptr<Gainer> create(shared_ptr<WavReader> input,
                                   shared_ptr<WavWriter> output,
                                   const vector<string> &params, int line_num);

  void convert() override;
  static string get_name() { return "Gainer"; }
  static string get_desc() {
    return "Increase/decrease audio file volume in specified time interval";
  }
  static string get_command() { return "gain"; }
  static string get_usage() { return get_command() + " [start] [end]"; };
};

#endif
