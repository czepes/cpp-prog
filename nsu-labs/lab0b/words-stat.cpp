#include "words-stat.h"
#include <iostream>
#include <sstream>

WordsStat::WordsStat(const std::string input_file) {
  input.open(input_file);
  total = 0;
}

WordsStat::~WordsStat() {
  if (input.is_open()) {
    input.close();
  }
}

const std::map<std::string, WordsFreq> &WordsStat::get_freq_map() const {
  return freq_map;
}

const WordsFreq WordsStat::get_freq(const std::string word) const {
  return freq_map.at(word);
}

const int WordsStat::get_total() const { return total; }

void WordsStat::count_words() {
  if (!input.is_open()) {
    std::cerr << "Error: Input file is not open" << std::endl;
    return;
  }

  std::string line;
  std::string word;

  while (std::getline(input, line)) {
    std::stringstream words(format_line(line));

    while (words >> word) {
      freq_map[word].first++;
      total++;
    }
  }

  for (auto [word, word_data] : freq_map) {
    word_data.second = (double)word_data.first / total * 100;
    freq_map[word] = word_data;
  }
}

void WordsStat::open(std::string input_file) {
  this->close();
  input.open(input_file);
}

void WordsStat::close() {
  if (input.is_open()) {
    input.close();
  }
}

std::string WordsStat::format_line(std::string line) const {
  for (char &ch : line) {
    if (ispunct(ch)) {
      ch = ' ';
    } else if (isupper(ch)) {
      ch = tolower(ch);
    }
  }

  return line;
}
