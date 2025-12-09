#include "muter.h"
#include "../config-parser.h"
#include <stdexcept>

unique_ptr<Muter> Muter::create(shared_ptr<WavReader> input,
                                shared_ptr<WavWriter> output,
                                const vector<string> &params, int line_num) {
  double start_time = 0;
  double end_time = -1;
  double max_time = input->get_duration();

  if (params.size() >= 1) {
    start_time = ConfigParser::parse_time_param(params[0], max_time, line_num);
  }
  if (params.size() >= 2) {
    end_time = ConfigParser::parse_time_param(params[1], max_time, line_num);
  }

  if (end_time != -1 && start_time > end_time) {
    throw runtime_error("Line " + to_string(line_num) +
                        ": Muter start time cannot be greater than end time");
  }

  return make_unique<Muter>(input, output, start_time, end_time);
}

Muter::Muter(shared_ptr<WavReader> input, shared_ptr<WavWriter> output,
             double start_time, double end_time)
    : Converter(input, output), start_time(start_time), end_time(end_time) {};

void Muter::convert() {
  if (!input || !output) {
    return;
  }

  const size_t SAMPLES_SIZE = input->seconds_to_sample(1);
  audio_buffer_t samples(SAMPLES_SIZE);

  const size_t start = input->seconds_to_sample(start_time);
  const size_t end = end_time < 0 ? input->get_total_samples()
                                  : input->seconds_to_sample(end_time);

  input->reset();

  while (output->get_current_pos() < start) {
    size_t to_read = min(SAMPLES_SIZE, start - output->get_current_pos());
    size_t read = input->read(samples, to_read);

    if (read == 0) {
      break;
    }

    output->write(samples);
  }

  size_t mute_samples = end - start;
  if (mute_samples > 0) {
    input->skip(mute_samples);
    output->write_silence(mute_samples);
  }

  while (!input->eof()) {
    size_t read = input->read(samples, SAMPLES_SIZE);
    if (read == 0) {
      break;
    }
    output->write(samples);
  }
}
