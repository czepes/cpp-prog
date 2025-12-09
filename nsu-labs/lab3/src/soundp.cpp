#include "soundp.h"
#include "config-parser.h"
#include "converter.h"
#include "wav.h"

#include <memory>
#include <stdexcept>

void SoundProcessor::process() {
  string temp_out = tmp1;
  string temp_in = files[1];

  auto output = make_shared<WavWriter>(temp_out);
  auto input = make_shared<WavReader>(temp_in);

  ParsedCommand command;

  while (parser.read_next_command(command)) {
    if (command.is_comment()) {
      continue;
    }

    auto converter = ConverterFactory::create(command.command, input, output,
                                              command.params, command.line_num);

    converter->convert();

    input->close();
    output->close();

    temp_in = temp_out;
    temp_out = (temp_in == tmp1) ? tmp2 : tmp1;

    input = make_shared<WavReader>(temp_in);
    output = make_shared<WavWriter>(temp_out);
  }

  if (rename(temp_in.c_str(), files[0].c_str()) != 0) {
    throw runtime_error("Failed to rename final output file");
  }
}

SoundProcessor::~SoundProcessor() {
  remove(tmp1.c_str());
  remove(tmp2.c_str());
}
