#include "soundp.h"

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>

void Muter::convert() {
  if (!src->is_open() || !dest->is_open()) {
    return;
  }

  const bool mute_until_end = end == -1;
  audio_t silence;
  silence.fill(0);
  audio_t samples;

  int i = 0;
  for (; i < start; i++) {
    src->read(samples);
    dest->write(samples);
  }
  for (; (mute_until_end || i < end) && !src->eof(); i++) {
    dest->write(silence);
    if (!src->eof()) {
      src->read(samples);
    }
  }
  while (!src->eof()) {
    src->read(samples);
    dest->write(samples);
  }

  src->close();
  dest->close();
}

void Mixer::convert() {
  if (!src->is_open() || !dest->is_open()) {
    return;
  }

  bool same = src->get_path() == src2->get_path();
  const bool mix_until_end = end == -1;
  audio_t samples;
  audio_t samples2;

  int i = 0;
  for (; i < start; i++) {
    src->read(samples);
    dest->write(samples);
  }
  for (; (mix_until_end || i < end) && !src->eof() && !src2->eof(); i++) {
    src->read(samples);

    if (!same) {
      src2->read(samples2);
      for (int j = 0; j < (int)samples.size(); j++) {
        samples[j] = samples[j] / 2 + samples2[j] / 2;
      }
    }

    dest->write(samples);
  }
  while (!src->eof()) {
    src->read(samples);
    dest->write(samples);
  }

  src->close();
  dest->close();
}

void Cropper::convert() {
  if (!src->is_open() || !dest->is_open()) {
    return;
  }

  const bool crop_to_end = end == -1;
  audio_t samples;

  int i = 0;
  for (; i < start && !src->eof(); i++) {
    src->read(samples);
  }
  for (; (crop_to_end || i < end) && !src->eof(); i++) {
    src->read(samples);
    dest->write(samples);
  }
}

void Gainer::convert() {
  if (!src->is_open() || !dest->is_open()) {
    return;
  }

  const bool gain_to_end = end == -1;
  audio_t samples;

  const int32_t mins = numeric_limits<sample_t>::min();
  const int32_t maxs = numeric_limits<sample_t>::max();

  int i = 0;
  for (; i < start && !src->eof(); i++) {
    src->read(samples);
    dest->write(samples);
  }
  for (; (gain_to_end || i < end) && !src->eof(); i++) {
    src->read(samples);

    for (int j = 0; j < (int)samples.size(); j++) {
      sample_t sample = samples[j];
      int32_t gained = sample * factor;
      if (sample < 0) {
        samples[j] = max(gained, mins);
      } else {
        samples[j] = min(gained, maxs);
      }
    }
    // for (sample_t &sample : samples) {
    //   sample_t gained = sample * factor;
    //
    //   if (sample < 0) {
    //     sample = gained > sample ? mins : gained;
    //   } else {
    //     sample = gained < sample ? maxs : gained;
    //   }
    // }

    dest->write(samples);
  }
  while (!src->eof()) {
    src->read(samples);
    dest->write(samples);
  }
}

void SoundProcessor::process() {
  string outfile = tmp1;
  string infile = files[1];

  shared_ptr<WavWriter> out = make_shared<WavWriter>(outfile);
  shared_ptr<WavReader> in = make_shared<WavReader>(infile);

  do {
    unique_ptr<Converter> converter = parser.parse(in, out);

    if (!converter) {
      continue;
    }

    converter->convert();

    if (parser.eof()) {
      break;
    }

    infile = outfile;
    outfile = infile == tmp1 ? tmp2 : tmp1;
    in = make_shared<WavReader>(infile);
    out = make_shared<WavWriter>(outfile);
  } while (!parser.eof());

  const string command = "mv " + infile + " " + files[0];
  int result = system(command.c_str());
  if (result != 0) {
    throw runtime_error("Failed to rename output file");
  }
}

SoundProcessor::~SoundProcessor() {
  remove(tmp1.c_str());
  remove(tmp2.c_str());
}

void ConfigParser::parse_range(stringstream &ss, int &start, int &end,
                               const double maxv) {
  const string line = "Line " + to_string(line_idx) + ": ";
  start = 0;
  end = -1;
  if (!ss.eof()) {
    ss >> start;
    if (start < 0 || start >= maxv) {
      throw runtime_error(line + "Invalid start value " + to_string(start) +
                          ":\nExpected to be in [0, " + to_string(maxv) + "]");
    }
  }
  if (!ss.eof()) {
    ss >> end;
    if (end < 0 || end >= maxv) {
      throw runtime_error(line + "Invalid end value " + to_string(end) +
                          ":\nExpected to be in [0, " + to_string(maxv) + "]");
    }
  }
  if (end != -1 && start > end) {
    throw runtime_error(line + "Start = " + to_string(start) + " > " +
                        to_string(end) + " = end");
  }
}

void ConfigParser::parse_file(stringstream &ss, int &file_idx) {
  const string line = "Line " + to_string(line_idx) + ": ";
  if (!ss.eof()) {
    string arg;
    ss >> arg;
    if (arg[0] != '$') {
      throw runtime_error(line + "Missing input file link");
    }
    try {
      file_idx = stoi(arg.substr(1));
    } catch (const exception &e) {
      throw runtime_error(line + "Failed to convert " + arg +
                          " to input file index:\n" + e.what());
    }
    if (file_idx < 1 || file_idx >= (int)files.size()) {
      throw runtime_error(line + "Invalid input file index: " + arg +
                          "\nExpected to be in [1, " +
                          to_string(files.size() - 1) + "]");
    }
  } else {
    throw runtime_error(line + ": Missing input file link");
  }
}

void ConfigParser::parse_factor(stringstream &ss, double &factor,
                                const double minv, const double maxv) {
  const string line = "Line " + to_string(line_idx) + ": ";
  const string range = "Expected to be in [" + to_string(minv) + ", " +
                       (maxv == -1 ? "inf)" : (to_string(maxv) + "]"));
  if (!ss.eof()) {
    ss >> factor;
    if (factor < minv || (maxv != -1 && factor > maxv)) {
      throw runtime_error(line + "Invalid factor value = " + to_string(factor) +
                          ":\n" + range);
    }
  } else {
    throw runtime_error(line + ": Missing factor value:\n" + range);
  }
}

unique_ptr<Converter> ConfigParser::parse(shared_ptr<WavReader> in,
                                          shared_ptr<WavWriter> out) {
  const string line = "Line " + to_string(line_idx) + ": ";
  unique_ptr<Converter> converter = nullptr;
  string option;
  string prompt;

  const double duration = in->get_duration();
  int start = 0;
  int end = -1;

  getline(*input, prompt);
  if (prompt.empty() || prompt.find_first_not_of(' ') == string::npos) {
    line_idx++;
    return nullptr;
  }

  if (prompt[0] == '#') {
    line_idx++;
    return nullptr;
  }

  stringstream ss(prompt);
  ss >> option;

  if (option == "mute") {
    parse_range(ss, start, end, duration);
    converter = Muter::fabricate(in, out, start, end);
  } else if (option == "mix") {
    int file_idx;
    parse_file(ss, file_idx);

    unique_ptr<WavReader> in2 = make_unique<WavReader>(files[file_idx]);

    const double duration2 = in2->get_duration();
    const double min_duration = duration > duration2 ? duration2 : duration;

    parse_range(ss, start, end, min_duration);

    converter = Mixer::fabricate(in, std::move(in2), out, start, end);
  } else if (option == "crop") {
    parse_range(ss, start, end, duration);
    converter = Cropper::fabricate(in, out, start, end);
  } else if (option == "gain") {
    double factor;
    parse_factor(ss, factor);
    parse_range(ss, start, end, duration);
    converter = Gainer::fabricate(in, out, factor, start, end);
  } else {
    throw runtime_error(line + "Unknown option: " + option);
  }

  line_idx++;
  return converter;
}
