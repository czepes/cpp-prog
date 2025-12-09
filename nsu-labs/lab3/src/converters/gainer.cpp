#include "gainer.h"
#include "../config-parser.h"
#include <limits>

unique_ptr<Gainer> Gainer::create(shared_ptr<WavReader> input,
                                  shared_ptr<WavWriter> output,
                                  const vector<string> &params, int line_num) {
  double factor = 1;
  double start_time = 0;
  double end_time = -1;

  if (params.size() >= 1) {
    factor = ConfigParser::parse_factor_param(params[0], 0, 10, line_num);
  }

  double max_time = input->get_duration();

  if (params.size() >= 2) {
    start_time = ConfigParser::parse_time_param(params[1], max_time, 0);
  }

  if (params.size() >= 3) {
    end_time = ConfigParser::parse_time_param(params[2], max_time, 0);
  }

  if (end_time != -1 && start_time > end_time) {
    throw runtime_error("Gainer start time cannot be greater than end time");
  }

  return make_unique<Gainer>(input, output, factor, start_time, end_time);
};

Gainer::Gainer(shared_ptr<WavReader> input, shared_ptr<WavWriter> output,
               double factor, double start_time, double end_time)
    : Converter(input, output), factor(factor), start_time(start_time),
      end_time(end_time) {};

void Gainer::convert() {
  if (!input || !output) {
    return;
  }

  const size_t SAMPLES_SIZE = input->seconds_to_sample(1);
  audio_buffer_t samples(SAMPLES_SIZE);

  const size_t start = input->seconds_to_sample(start_time);
  const size_t end = end_time < 0 ? input->get_total_samples()
                                  : input->seconds_to_sample(end_time);

  const int32_t mins = numeric_limits<sample_t>::min();
  const int32_t maxs = numeric_limits<sample_t>::max();

  input->reset();

  while (output->get_current_pos() < start) {
    size_t to_read = min(SAMPLES_SIZE, start - output->get_current_pos());
    size_t read = input->read(samples, to_read);

    if (read == 0) {
      break;
    }

    output->write(samples);
  }

  while (output->get_current_pos() < end) {
    size_t to_read = min(SAMPLES_SIZE, end - output->get_current_pos());
    size_t read = input->read(samples, to_read);

    if (read == 0) {
      break;
    }

    for (sample_t &sample : samples) {
      int32_t gained = sample * factor;
      if (sample < 0) {
        sample = max(gained, mins);
      } else {
        sample = min(gained, maxs);
      }
    }

    output->write(samples);
  }

  while (!input->eof()) {
    size_t read = input->read(samples, SAMPLES_SIZE);
    if (read == 0) {
      break;
    }
    output->write(samples);
  }

  while (!input->eof()) {
    input->read(samples, SAMPLES_SIZE);
    output->write(samples);
  }
}
