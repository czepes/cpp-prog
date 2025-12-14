#include "wav.h"
#include "errors.h"
#include <cstring>
#include <iostream>
#include <string>

bool WavFile::validate_header(WavHeader &header) {
  WavHeader valid;
  return memcmp(header.riff, valid.riff, sizeof(valid.riff)) == 0 &&
         memcmp(header.wave, valid.wave, sizeof(valid.wave)) == 0 &&
         header.audio_format == valid.audio_format &&
         header.num_channels == valid.num_channels &&
         header.sample_rate == valid.sample_rate &&
         header.bits_per_sample == valid.bits_per_sample;
}

size_t WavFile::get_total_samples() const {
  return header.data_size / sizeof(sample_t);
}

size_t WavFile::get_remaining_samples() const {
  return get_total_samples() - current_pos;
}

double WavFile::get_duration() const {
  return static_cast<double>(get_total_samples()) / header.sample_rate;
}

size_t WavFile::seconds_to_sample(double seconds) const {
  return static_cast<size_t>(seconds * header.sample_rate);
}

double WavFile::sample_to_seconds(size_t sample_index) const {
  return static_cast<double>(sample_index) / header.sample_rate;
}

void WavFile::open(const string &path, ios::openmode mode) {
  close();
  this->path = path;

  stream.open(path, ios::binary | mode);

  if (!stream || !stream.is_open()) {
    throw WavError("Failed to open file " + path);
  }

  data_start = sizeof(WavHeader);
  current_pos = 0;
}
void WavFile::close() {
  if (stream.is_open()) {
    stream.close();
  }
}
void WavFile::print_header(ostream &out) {
  out << header.riff << endl
      << header.file_size << endl
      << header.wave << endl
      << header.fmt << endl
      << header.fmt_size << endl
      << header.audio_format << endl
      << header.num_channels << endl
      << header.sample_rate << endl
      << header.byte_rate << endl
      << header.block_align << endl
      << header.bits_per_sample << endl
      << header.data << endl
      << header.data_size << endl;
}

WavReader::WavReader(const string &path) { open(path); }
WavReader::~WavReader() { close(); }

void WavReader::open(const string &path) {
  if (this->path == path) {
    return;
  }

  this->path = path;
  WavFile::open(path, ios::in);

  size_t shift = sizeof(header.data) + sizeof(header.data_size);
  stream.read(reinterpret_cast<char *>(&header), sizeof(header) - shift);

  if (!validate_header(header)) {
    throw WavError("Unsupproted WAV format for file: " + path);
  }

  while (true) {
    char chunk_id[chunk_id_size];
    streampos current = stream.tellg();

    stream.read(chunk_id, sizeof(chunk_id));
    if (!stream) {
      cerr << "Failed to read chunk_id" << endl;
      break;
    }

    if (memcmp(chunk_id, "data", chunk_id_size) == 0) {
      data_start = current;
      stream.read(reinterpret_cast<char *>(&header.data_size),
                  sizeof(header.data_size));
      break;
    }

    chunk_size_t chunk_size;
    stream.read(reinterpret_cast<char *>(&chunk_size), sizeof(chunk_size));
    stream.seekg(chunk_size, ios::cur);
  }

  current_pos = 0;
  data_start = sizeof(header);
  stream.seekg(data_start);
}

size_t WavReader::read(audio_buffer_t &samples, size_t num_samples) {
  if (!stream.is_open()) {
    throw WavError("Tried to read from " + path + " , but stream is closed");
  }

  if (stream.eof()) {
    return 0;
  }

  size_t to_read = min(get_remaining_samples(), num_samples);

  if (to_read == 0) {
    samples.clear();
    return 0;
  }

  samples.resize(to_read);

  stream.read(reinterpret_cast<char *>(samples.data()),
              to_read * sizeof(sample_t));

  if (stream.fail() && !stream.eof()) {
    throw WavError("Failed to read from " + path);
  }

  size_t samples_read = stream.gcount() / sizeof(sample_t);
  samples.resize(samples_read);
  current_pos += samples_read;

  return samples_read;
}

size_t WavReader::read(audio_buffer_t &samples) {
  return read(samples, get_remaining_samples());
}

size_t WavReader::read(audio_buffer_t &samples, double start, double end) {
  size_t start_sample = seconds_to_sample(start);
  size_t end_sample = seconds_to_sample(end);

  if (start_sample >= get_total_samples()) {
    samples.clear();
    return 0;
  }

  if (end_sample <= start_sample) {
    return 0;
  }

  if (end_sample > get_total_samples()) {
    end_sample = get_total_samples();
  }

  skip_to(start_sample);
  size_t samples_read = read(samples, end_sample - start_sample);

  return samples_read;
}

void WavReader::skip(size_t num_samples) {
  size_t to_skip = min(get_remaining_samples(), num_samples);
  stream.seekg(to_skip * sizeof(sample_t), ios::cur);
  current_pos += to_skip;
}

void WavReader::skip_to(size_t sample_pos) {
  if (sample_pos == current_pos) {
    return;
  }
  size_t to_skip = min(get_total_samples(), sample_pos);
  stream.seekg(data_start + to_skip * sizeof(sample_t));
  current_pos = sample_pos;
}

void WavReader::reset() {
  stream.seekg(data_start, ios::beg);
  current_pos = 0;
}

WavWriter::WavWriter(const string &path) { open(path); }
WavWriter::~WavWriter() { close(); }

void WavWriter::open(const string &path) {
  if (this->path == path) {
    return;
  }
  WavFile::open(path, ios::out | ios::trunc);
  stream.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
}

void WavWriter::close() {
  update_header();
  WavFile::close();
}

void WavWriter::update_header() {
  if (!stream.is_open()) {
    return;
  }

  header.data_size = current_pos * sizeof(sample_t);
  header.file_size = sizeof(WavHeader) - 8 + header.data_size;

  streampos current = stream.tellp();
  stream.seekp(0);
  stream.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
  stream.seekp(current);
}

void WavWriter::write(sample_t sample) {
  if (!stream.is_open()) {
    throw WavError("Tried to write to WavWriter, but stream is closed");
  }

  stream.write(reinterpret_cast<char *>(&sample), sizeof(sample_t));

  if (stream.fail()) {
    throw WavError("Failed to write to " + path);
  }

  current_pos++;
}

void WavWriter::write(const audio_buffer_t &samples) {
  if (!stream.is_open()) {
    throw WavError("Tried to write to WavWriter, but stream is closed");
  }

  if (samples.empty()) {
    return;
  }

  size_t num_samples = samples.size();
  stream.write(reinterpret_cast<const char *>(samples.data()),
               num_samples * sizeof(sample_t));

  if (stream.fail()) {
    throw WavError("Failed to write to " + path);
  }

  current_pos += num_samples;
}

void WavWriter::write_silence(size_t num_samples) {
  if (!stream.is_open()) {
    throw WavError("Tried to write to WavWriter, but stream is closed");
  }

  if (num_samples == 0) {
    return;
  }

  audio_buffer_t silence(num_samples, 0);
  write(silence);
}
