#include "mixer.h"
#include "../config-parser.h"
#include <memory>
#include <stdexcept>

unique_ptr<Mixer> Mixer::create(shared_ptr<WavReader> input,
                                shared_ptr<WavWriter> output,
                                const vector<string> &params, int line_num) {
  double start_time = 0;
  double end_time = -1;
  double max_time = input->get_duration();

  if (params.size() < 1) {
    throw runtime_error("Line " + to_string(line_num) +
                        ": Missing file reference");
  }

  shared_ptr<WavReader> input2 = make_shared<WavReader>(params[0]);

  if (params.size() >= 2) {
    start_time = ConfigParser::parse_time_param(params[1], max_time, line_num);
  }
  if (params.size() >= 3) {
    end_time = ConfigParser::parse_time_param(params[2], max_time, line_num);
  }

  if (end_time != -1 && start_time > end_time) {
    throw runtime_error("Line " + to_string(line_num) +
                        ": Muter start time cannot be greater than end time");
  }

  return make_unique<Mixer>(input, output, input2, start_time, end_time);
}
Mixer::Mixer(shared_ptr<WavReader> input, shared_ptr<WavWriter> output,
             shared_ptr<WavReader> input2, double start_time, double end_time)
    : Converter(input, output), input2(input2), start_time(start_time),
      end_time(end_time) {};

void Mixer::convert() {
  if (!input || !output) {
    return;
  }

  const bool same = input->get_path() == input2->get_path();
  const size_t SAMPLES_SIZE = input->seconds_to_sample(1);
  audio_buffer_t samples(SAMPLES_SIZE);
  audio_buffer_t samples2(SAMPLES_SIZE);

  const size_t start = input->seconds_to_sample(start_time);
  const size_t end = end_time < 0 ? input->get_total_samples()
                                  : input->seconds_to_sample(end_time);

  input->reset();
  if (!same) {
    input2->reset();
  }

  while (output->get_current_pos() < start) {
    size_t to_read = min(SAMPLES_SIZE, start - output->get_current_pos());
    size_t read = input->read(samples, to_read);

    if (read == 0) {
      break;
    }

    output->write(samples);
  }
  input2->skip_to(start);

  while (output->get_current_pos() < end) {
    size_t to_read = min(SAMPLES_SIZE, end - output->get_current_pos());
    size_t read = input->read(samples, to_read);

    if (read == 0) {
      break;
    }

    if (!same && !input2->eof()) {
      size_t read2 = input2->read(samples2, to_read);
      for (size_t j = 0; j < min(read, read2); j++) {
        samples[j] = samples[j] / 2 + samples2[j] / 2;
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
}
