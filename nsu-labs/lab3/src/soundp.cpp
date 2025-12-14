#include "soundp.h"
#include "config-parser.h"
#include "converter.h"
#include "errors.h"
#include "wav.h"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

SoundProcessor::SoundProcessor(const string &config_file,
                               const vector<string> &files)
    : parser(config_file, files), files(files) {
  if (files.size() < 2) {
    throw SoundProcessorError(
        "Expected at least 2 files: output.wav and input.wav");
  }

  fs::path tmp_dir = fs::temp_directory_path();

  tmp1 = (tmp_dir / generate_temp_filename("tmp1", ".wav")).string();
  tmp2 = (tmp_dir / generate_temp_filename("tmp2", ".wav")).string();
}

SoundProcessor::~SoundProcessor() {
  remove(tmp1.c_str());
  remove(tmp2.c_str());
}

void SoundProcessor::process() {
  string current_output = tmp1;
  string current_input = files[1];

  auto input = make_shared<WavReader>(current_input);
  auto output = make_shared<WavWriter>(current_output);

  ParsedCommand command;

  while (parser.read_next_command(command)) {
    if (command.is_comment()) {
      continue;
    }

    auto converter = ConverterFactory::create(command.command, output, input,
                                              command.params, command.line_num);

    converter->convert();

    input->close();
    output->close();

    swap(current_input, current_output);
    current_output = (current_input == tmp1) ? tmp2 : tmp1;

    input->open(current_input);
    output->open(current_output);
  }

  try {
    fs::copy_file(current_input, files[0],
                  fs::copy_options::overwrite_existing);
  } catch (fs::filesystem_error &e) {
    throw SoundProcessorError("Failed to copy final output file: " +
                              string(e.what()));
  }
}
