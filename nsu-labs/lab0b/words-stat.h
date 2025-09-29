#ifndef WORDS_STAT
#define WORDS_STAT

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using WordsFreq = std::pair<int, double>;
using WordsData = std::pair<std::string, WordsFreq>;

class WordsStat {
private:
  int total;
  std::ifstream input;
  std::map<std::string, WordsFreq> freq_map;
  std::string format_line(std::string line) const;

public:
  WordsStat(const std::string input_file);
  ~WordsStat();
  const std::map<std::string, WordsFreq> &get_freq_map() const;
  const WordsFreq get_freq(const std::string word) const;
  const int get_total() const;
  void count_words();
  void open(std::string input_file);
  void close();
};

#endif
