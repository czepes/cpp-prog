#include "cropper.h"
#include "../config-parser.h"
#include "../errors.h"

unique_ptr<Cropper> Cropper::create(shared_ptr<WavWriter> output,
                                    shared_ptr<WavReader> input,
                                    const vector<string> &params,
                                    int line_num) {

  double start_time = 0;
  double end_time = 0;
  double max_time = input->get_duration();

  if (params.size() >= 1) {
    start_time = ConfigParser::parse_time_param(params[0], max_time, 0);
  }

  if (params.size() >= 2) {
    end_time = ConfigParser::parse_time_param(params[1], max_time, 0);
  }

  if (end_time != -1 && start_time > end_time) {
    throw ConverterError(line_num,
                         "Cropper start time cannot be greater than end time");
  }

  return make_unique<Cropper>(output, input, start_time, end_time);
};

Cropper::Cropper(shared_ptr<WavWriter> output, shared_ptr<WavReader> input,
                 double start_time, double end_time)
    : Converter(output, input), start_time(start_time), end_time(end_time) {};

void Cropper::convert() {
  if (!input || !output) {
    return;
  }

  const size_t SAMPLES_SIZE = input->seconds_to_sample(1);
  audio_buffer_t samples(SAMPLES_SIZE);

  const size_t start = input->seconds_to_sample(start_time);
  const size_t end = end_time < 0 ? input->get_total_samples()
                                  : input->seconds_to_sample(end_time);

  input->reset();

  input->skip_to(start);
  while (output->get_current_pos() < end) {
    size_t read = input->read(samples, SAMPLES_SIZE);
    if (read == 0) {
      break;
    }
    output->write(samples);
  }
}
