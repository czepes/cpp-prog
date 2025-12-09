#ifndef SOUNDP_WAV
#define SOUNDP_WAV

// TODO: remove
#include <iostream>

#include <cstdint>
#include <fstream>
#include <vector>

#define SAMPLE_RATE 44100

using namespace std;

using sample_t = int16_t;
const size_t audio_size = SAMPLE_RATE;
using audio_buffer_t = vector<sample_t>;

#pragma pack(push, 1)
struct WavHeader {
  // RIFF chunk
  char riff[4] = {'R', 'I', 'F', 'F'};
  uint32_t file_size = 36;
  char wave[4] = {'W', 'A', 'V', 'E'};

  // fmt sub-chunk
  char fmt[4] = {'f', 'm', 't', ' '};
  uint32_t fmt_size = 16;
  uint16_t audio_format = 1;
  uint16_t num_channels = 1;
  uint32_t sample_rate = SAMPLE_RATE;
  uint32_t byte_rate = SAMPLE_RATE * 8;
  uint16_t block_align = 1 * 2;
  uint16_t bits_per_sample = 16;

  // data sub-chunk
  char data[4] = {'d', 'a', 't', 'a'};
  uint32_t data_size = 0;
};
#pragma pack(pop)

class WavFile {
protected:
  WavHeader header;
  string path;
  fstream stream;
  size_t data_start;
  size_t current_pos = 0;

public:
  static bool validate_header(WavHeader &header);

  const WavHeader &get_header() const { return header; }
  const string &get_path() const { return path; }

  size_t get_total_samples() const {
    return header.data_size / sizeof(sample_t);
  }
  size_t get_current_pos() const { return current_pos; }
  size_t get_remaining_samples() const {
    return get_total_samples() - current_pos;
  }

  double get_duration() const {
    return static_cast<double>(get_total_samples()) / header.sample_rate;
  }

  size_t seconds_to_sample(double seconds) const {
    return static_cast<size_t>(seconds * header.sample_rate);
  }
  double sample_to_seconds(size_t sample_index) const {
    return static_cast<double>(sample_index) / header.sample_rate;
  }

  bool is_open() const { return stream.is_open(); }
  bool eof() const { return stream.eof() || get_remaining_samples() == 0; }

  void close() {
    if (stream.is_open()) {
      stream.close();
    }
  }

  void print_header(ostream &out) {
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

  virtual ~WavFile() = default;
};

class WavReader;
class WavWriter;

class WavReader : public WavFile {
public:
  explicit WavReader(const string &path);
  ~WavReader();

  size_t read(audio_buffer_t &samples, size_t num_samples);
  size_t read(audio_buffer_t &samples);
  size_t read(audio_buffer_t &samples, double start, double end);

  void skip(size_t num_samples);
  void skip_to(size_t sample_pos);
  void reset();

  size_t get_current_pos() const { return current_pos; }
};

class WavWriter : public WavFile {
public:
  explicit WavWriter(const string &path);
  ~WavWriter();

  void write(sample_t sample);
  void write(const audio_buffer_t &samples);
  void write_silence(size_t num_samples);

  size_t get_current_pos() const { return current_pos; }

  void update_header();
  void close();
};

#endif
