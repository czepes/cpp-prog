#include <stdexcept>

#define CONFIG_PARSER_ERROR_CODE 1
#define CONVERTER_ERROR_CODE 2
#define SOUND_PROCESSOR_ERROR_CODE 3
#define WAV_ERROR_CODE 4
#define ARGPARSER_ERROR_CODE 5

using namespace std;

class SoundError : public runtime_error {
public:
  SoundError(const string &msg) : runtime_error(msg) {};
  virtual ~SoundError() = default;
  virtual int code() const = 0;
};

class SoundLineError : public SoundError {
private:
  size_t line_num = 0;

public:
  SoundLineError(const string &msg) : SoundError(msg) {};
  SoundLineError(size_t line_num, const string &msg)
      : SoundError("Line " + to_string(line_num) + ": " + msg),
        line_num(line_num) {};
  size_t get_line_number() const { return line_num; }
};

class ConfigParserError : public SoundLineError {
public:
  ConfigParserError(const string &msg) : SoundLineError(msg) {};
  ConfigParserError(size_t line_num, const string &msg)
      : SoundLineError(line_num, msg) {};

  int code() const override { return CONFIG_PARSER_ERROR_CODE; };
};

class ConverterError : public SoundLineError {
public:
  ConverterError(const string &msg) : SoundLineError(msg) {};
  ConverterError(size_t line_num, const string &msg)
      : SoundLineError(line_num, msg) {};
  int code() const override { return CONVERTER_ERROR_CODE; }
};

class SoundProcessorError : public SoundError {
public:
  SoundProcessorError(const string &msg) : SoundError(msg) {};
  int code() const override { return SOUND_PROCESSOR_ERROR_CODE; }
};

class WavError : public SoundError {
public:
  WavError(const string &msg) : SoundError(msg) {};
  int code() const override { return WAV_ERROR_CODE; }
};

class ArgParserError : public SoundError {
public:
  ArgParserError(const string &msg) : SoundError(msg) {};

  int code() const override { return ARGPARSER_ERROR_CODE; }
};
