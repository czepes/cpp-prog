#include "wav.h"
#include <cstdint>
#include <cstring>
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

WavReader::WavReader(const string &path) {
  this->path = path;
  stream.open(path, ios::binary | ios::in);

  if (!stream) {
    throw invalid_argument("Failed to open file: " + path);
  }

  size_t data_start = sizeof(WavHeader) - 8;
  stream.read(reinterpret_cast<char *>(&header), data_start);

  if (!validate_header(header)) {
    throw runtime_error("Unsupproted WAV format for file: " + path);
  }

  while (true) {
    char chunk_id[4];
    streampos current = stream.tellg();

    stream.read(chunk_id, sizeof(chunk_id));
    if (!stream) {
      cerr << "Failed to read chunk_id" << endl;
      break;
    }

    if (memcmp(chunk_id, "data", 4) == 0) {
      data_start = current;
      stream.read(reinterpret_cast<char *>(&header.data_size),
                  sizeof(uint32_t));
      break;
    }

    uint32_t chunk_size;
    stream.read(reinterpret_cast<char *>(&chunk_size), sizeof(uint32_t));
    stream.seekg(chunk_size, ios::cur);
  }

  current_pos = 0;
  data_start += 8;
  stream.seekg(data_start);
}

WavReader::~WavReader() { close(); }

size_t WavReader::read(audio_buffer_t &samples, size_t num_samples) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to read from " + path +
                        " , but stream is closed");
  }

  if (stream.eof()) {
    return 0;
  }

  // TODO: count n = eof_pos - cur_pos?
  size_t to_read = min(get_remaining_samples(), num_samples);

  if (to_read == 0) {
    samples.clear();
    return 0;
  }

  samples.resize(to_read);

  stream.read(reinterpret_cast<char *>(samples.data()),
              to_read * sizeof(sample_t));

  if (stream.fail() && !stream.eof()) {
    throw runtime_error("Failed to read from " + path);
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

  if (end_sample <= start_sample || end_sample > get_total_samples()) {
    end_sample = get_total_samples();
  }

  skip_to(start_sample);
  size_t samples_read = read(samples, end_sample - start_sample);

  return samples_read;
}

void WavReader::skip(size_t num_samples) {
  size_t to_skip = min(get_remaining_samples(), num_samples);
  stream.seekg(to_skip, ios::cur);
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

WavWriter::WavWriter(const string &path) {
  this->path = path;
  stream.open(path, ios::binary | ios::out | ios::trunc);

  if (!stream) {
    throw invalid_argument("Failed to open file: " + path);
  }

  stream.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
  data_start = sizeof(WavHeader);
  current_pos = 0;
}

WavWriter::~WavWriter() { close(); }

void WavWriter::write(sample_t sample) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write to WavWriter, but stream is closed");
  }

  stream.write(reinterpret_cast<char *>(&sample), sizeof(sample_t));

  if (stream.fail()) {
    throw runtime_error("Failed to write to " + path);
  }

  current_pos++;
}

void WavWriter::write(const audio_buffer_t &samples) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write to WavWriter, but stream is closed");
  }

  if (samples.empty()) {
    return;
  }

  size_t num_samples = samples.size();
  stream.write(reinterpret_cast<const char *>(samples.data()),
               num_samples * sizeof(sample_t));

  if (stream.fail()) {
    throw runtime_error("Failed to write to " + path);
  }

  current_pos += num_samples;
}

void WavWriter::write_silence(size_t num_samples) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write to WavWriter, but stream is closed");
  }

  if (num_samples == 0) {
    return;
  }

  audio_buffer_t silence(num_samples, 0);
  write(silence);
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
