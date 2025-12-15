#include "converter.h"
#include "./converters/cropper.h"
#include "./converters/gainer.h"
#include "./converters/mixer.h"
#include "./converters/muter.h"
#include "errors.h"
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
    register_converter<Gainer>();
    register_converter<Cropper>();
    register_converter<Mixer>();
    register_converter<Muter>();
  }

  initialized = true;
}

template <class ConverterImpl> void ConverterFactory::register_converter() {
  const string command = ConverterImpl::get_command();
  const ConverterCreator creator = ConverterImpl::create;
  const string description =
      "  " + ConverterImpl::get_desc() + "\n  " + ConverterImpl::get_usage();

  get_creator_registry()[command] = creator;
  get_description_registry()[command] = description;
}

unique_ptr<Converter> ConverterFactory::create(const string &command,
                                               shared_ptr<WavWriter> output,
                                               shared_ptr<WavReader> input,
                                               const vector<string> &params,
                                               int line_num) {
  auto &registry = get_creator_registry();
  auto it = registry.find(command);

  if (it == registry.end()) {
    throw ConverterError("Unknown converter type: " + command);
  }

  return it->second(output, input, params, line_num);
}

string ConverterFactory::help() {
  stringstream ss;
  ss << "Available converters:\n";

  auto &desc_registry = get_description_registry();
  for (const auto &pair : desc_registry) {
    ss << pair.first << ":\n";
    ss << pair.second << "\n";
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
