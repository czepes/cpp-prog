#include "converter.h"
#include "./converters/cropper.h"
#include "./converters/gainer.h"
#include "./converters/mixer.h"
#include "./converters/muter.h"
#include <memory>
#include <sstream>
#include <unordered_map>

unordered_map<string, ConverterCreator> &
ConverterFactory::get_creator_registry() {
  static unordered_map<string, ConverterCreator> registry;
  return registry;
}

unordered_map<string, string> &ConverterFactory::get_description_registry() {
  static unordered_map<string, string> registry;
  return registry;
}

void ConverterFactory::init() {
  static bool initialized = false;

  if (!initialized) {
    register_converter(Muter::get_command(), Muter::create,
                       Muter::get_desc() + "\n" + Muter::get_usage());
    register_converter(Mixer::get_command(), Mixer::create,
                       Mixer::get_desc() + "\n" + Muter::get_usage());
    register_converter(Cropper::get_command(), Cropper::create,
                       Cropper::get_desc() + "\n" + Cropper::get_usage());
    register_converter(Gainer::get_command(), Gainer::create,
                       Gainer::get_desc() + "\n" + Gainer::get_usage());
  }

  initialized = true;
}

void ConverterFactory::register_converter(const string &command,
                                          ConverterCreator creator,
                                          const string &description) {
  get_creator_registry()[command] = creator;
  get_description_registry()[command] = description;
}

unique_ptr<Converter> ConverterFactory::create(const string &command,
                                               shared_ptr<WavReader> input,
                                               shared_ptr<WavWriter> output,
                                               const vector<string> &params,
                                               int line_num) {
  auto &registry = get_creator_registry();
  auto it = registry.find(command);

  if (it == registry.end()) {
    throw runtime_error("Unknown converter type: " + command);
  }

  return it->second(input, output, params, line_num);
}

string ConverterFactory::help() {
  stringstream ss;
  ss << "Available converters:\n";

  auto &desc_registry = get_description_registry();
  for (const auto &pair : desc_registry) {
    ss << pair.first << ":\n";
    ss << "  " << pair.second << "\n";
  }

  return ss.str();
}

string ConverterFactory::help(const string &command) {
  auto &desc_registry = get_description_registry();
  auto it = desc_registry.find(command);

  if (it == desc_registry.end()) {
    return "Unknown converter: " + command;
  }

  return it->second;
}
