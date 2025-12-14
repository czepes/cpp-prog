#ifndef CROPPER_H
#define CROPPER_H

#include "../converter.h"

class Cropper : public Converter {
private:
  double factor;
  double start_time;
  double end_time;

public:
  Cropper(shared_ptr<WavWriter> output, shared_ptr<WavReader> input,
          double start_time, double ent_time);

  static unique_ptr<Cropper> create(shared_ptr<WavWriter> output,
                                    shared_ptr<WavReader> input,
                                    const vector<string> &params, int line_num);

  void convert() override;
  static string get_name() { return "Cropper"; }
  static string get_desc() {
    return "Crop audio files in specified time interval";
  }
  static string get_command() { return "crop"; }
  static string get_usage() { return get_command() + " [start] [end]"; };
};

#endif
