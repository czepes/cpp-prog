#ifndef SOUNDP_WAV
#define SOUNDP_WAV

#include <cstdint>
#include <fstream>
#include <vector>

#define SAMPLE_RATE 44100

using namespace std;

using sample_t = int16_t;
const size_t audio_size = SAMPLE_RATE;
using audio_buffer_t = vector<sample_t>;

const size_t chunk_id_size = 4;
using chunk_size_t = int32_t;

#pragma pack(push, 1)
struct WavHeader {
  // RIFF chunk
  char riff[chunk_id_size] = {'R', 'I', 'F', 'F'};
  chunk_size_t file_size = 36;
  char wave[chunk_id_size] = {'W', 'A', 'V', 'E'};

  // fmt sub-chunk
  char fmt[chunk_id_size] = {'f', 'm', 't', ' '};
  chunk_size_t fmt_size = 16;
  uint16_t audio_format = 1;
  uint16_t num_channels = 1;
  uint32_t sample_rate = SAMPLE_RATE;
  uint32_t byte_rate = SAMPLE_RATE * 8;
  uint16_t block_align = 1 * 2;
  uint16_t bits_per_sample = 16;

  // data sub-chunk
  char data[chunk_id_size] = {'d', 'a', 't', 'a'};
  chunk_size_t data_size = 0;
};
#pragma pack(pop)

class WavFile {
protected:
  WavHeader header;
  string path;
  fstream stream;
  size_t data_start;
  size_t current_pos;

public:
  static bool validate_header(WavHeader &header);

  const WavHeader &get_header() const { return header; }
  const string &get_path() const { return path; }
  size_t get_current_pos() const { return current_pos; }
  size_t get_total_samples() const;
  size_t get_remaining_samples() const;
  double get_duration() const;

  size_t seconds_to_sample(double seconds) const;
  double sample_to_seconds(size_t sample_index) const;

  void open(const string &path, ios::openmode mode);
  void close();

  bool is_open() const { return stream.is_open(); }
  bool eof() const { return stream.eof() || get_remaining_samples() == 0; }

  void print_header(ostream &out);

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

  void open(const string &path);
};

class WavWriter : public WavFile {
public:
  explicit WavWriter(const string &path);
  ~WavWriter();

  void write(sample_t sample);
  void write(const audio_buffer_t &samples);
  void write_silence(size_t num_samples);

  void open(const string &path);
  void close();
  void update_header();
};

#endif
