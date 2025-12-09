#ifndef CONVERTER_H
#define CONVERTER_H

#include "wav.h"
#include <functional>
#include <memory>
#include <unordered_map>

using namespace std;

class Converter {
protected:
  shared_ptr<WavWriter> output;
  shared_ptr<WavReader> input;

public:
  Converter(shared_ptr<WavReader> input, shared_ptr<WavWriter> output)
      : output(output), input(input) {};
  virtual ~Converter() = default;
  virtual void convert() = 0;
  static string get_name();
  static string get_desc();
  static string get_command();
  static string get_usage();
};

using ConverterCreator =
    function<unique_ptr<Converter>(shared_ptr<WavReader>, shared_ptr<WavWriter>,
                                   const vector<string> &, int line_num)>;

class ConverterFactory {
private:
  static unordered_map<string, ConverterCreator> &get_creator_registry();
  static unordered_map<string, string> &get_description_registry();

  ConverterFactory() = delete;

public:
  static void init();

  static void register_converter(const string &command,
                                 ConverterCreator creator,
                                 const string &description);

  static unique_ptr<Converter> create(const string &command,
                                      shared_ptr<WavReader> input,
                                      shared_ptr<WavWriter> output,
                                      const vector<string> &params,
                                      int line_num);

  static string help();
  static string help(const string &command);
};

#endif
