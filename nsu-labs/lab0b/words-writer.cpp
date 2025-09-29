#include "words-writer.h"
#include <fstream>
#include <iostream>

WordsWriter::~WordsWriter() {
  if (output.is_open()) {
    output.close();
  }
}

WordsWriter::WordsWriter(std::string output_file, WordsStat *words_stat) {
  output.open(output_file);
  stat = words_stat;
}

void WordsWriter::close() {
  if (output.is_open()) {
    output.close();
  }
}

void WordsWriter::open(std::string output_file) {
  this->close();
  output.open(output_file);
}

bool WordsWriter::write(const std::vector<const std::string *> &order) {
  if (!output.is_open()) {
    std::cerr << "Error: Output file is not open" << std::endl;
    return false;
  }

  output << "word;amount;rate(%)" << std::endl;

  for (const auto word : order) {
    WordsFreq freq = stat->get_freq(*word);
    output << '"' << *word << '"' << ";" << freq.first << ";" << freq.second
           << std::endl;
  }

  return true;
}
