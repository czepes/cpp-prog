#include "wav.h"
#include <cstdint>
#include <cstring>

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

  data_start = sizeof(WavHeader) - 8;
  stream.read(reinterpret_cast<char *>(&header), data_start);

  if (!validate_header(header)) {
    throw runtime_error("Unsupproted WAV format for file: " + path);
  }

  char chunk_id[4];

  while (true) {
    streampos current = stream.tellg();

    stream.read(chunk_id, 4);
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

  stream.seekg(data_start + 8);
}

WavReader::~WavReader() { close(); }

size_t WavReader::read(audio_t &samples) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to read from WavReader, but stream is closed");
  }

  if (stream.eof()) {
    return 0;
  }

  stream.read(reinterpret_cast<char *>(samples.data()), sizeof(audio_t));

  size_t bytes_read = stream.gcount();
  size_t samples_read = bytes_read / sizeof(sample_t);

  if (samples_read < RATE) {
    for (size_t i = samples_read; i < RATE; i++) {
      samples[i] = 0;
    }
  }

  return samples_read;
}

WavReader &WavReader::operator>>(sample_t &sample) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to read from WavReader, but stream is closed");
  }

  if (stream.eof()) {
    return *this;
  }

  stream.read(reinterpret_cast<char *>(&sample), sizeof(sample_t));
  return *this;
}

WavReader &WavReader::operator>>(audio_t &samples) {
  read(samples);
  return *this;
}

WavReader &WavReader::operator>>(WavWriter &writer) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to read from WavReader to WavWriter, but "
                        "WavReader stream is closed");
  }

  if (!writer.is_open()) {
    throw runtime_error("Tried to read from WavReader to WavWriter, but "
                        "WavWriter stream is closed");
  }

  if (stream.eof() || writer.eof()) {
    return *this;
  }

  sample_t sample;
  stream.read(reinterpret_cast<char *>(&sample), sizeof(sample_t));
  writer << sample;

  return *this;
}

WavWriter::WavWriter(const string &path) {
  this->path = path;
  stream.open(path, ios::binary | ios::out);

  if (!stream) {
    throw invalid_argument("Failed to open file: " + path);
  }

  header = WavHeader();
  data_start = sizeof(WavHeader);
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

  streampos current = stream.tellp();
  header.file_size = sizeof(WavHeader) - 8 + header.data_size;

  stream.seekp(0);
  stream.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
  stream.seekp(current);
}

WavWriter::~WavWriter() { close(); }

void WavWriter::write(const audio_t &samples) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write to WavWriter, but stream is closed");
  }

  stream.write(reinterpret_cast<const char *>(samples.data()), sizeof(audio_t));
  header.data_size += sizeof(audio_t);
  header.file_size += sizeof(audio_t);
}

WavWriter &WavWriter::operator<<(const sample_t sample) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write to WavWriter, but stream is closed");
  }

  stream.write(reinterpret_cast<const char *>(&sample), sizeof(sample));
  header.data_size += sizeof(sample_t);
  header.file_size += sizeof(sample_t);

  return *this;
}

WavWriter &WavWriter::operator<<(const audio_t &samples) {
  write(samples);
  return *this;
}

WavWriter &WavWriter::operator<<(WavReader &reader) {
  if (!stream.is_open()) {
    throw runtime_error("Tried to write from WavReader to WavWriter, but "
                        "WavWriter stream is closed");
  }

  if (!reader.is_open()) {
    throw runtime_error("Tried to write from WavReader to WavWriter, but "
                        "WavReader stream is closed");
  }

  if (reader.eof()) {
    throw runtime_error("Tried to write from WavReader to WavWriter, but "
                        "WavReader stream is eof");
  }

  sample_t sample;
  reader >> sample;
  stream.write(reinterpret_cast<const char *>(&sample), sizeof(sample));
  header.data_size += sizeof(sample_t);
  header.file_size += sizeof(sample_t);

  return *this;
}
