#include "words-stat.h"

class WordsWriter {
private:
  std::ofstream output;
  WordsStat *stat;

public:
  ~WordsWriter();
  WordsWriter(std::string output_file, WordsStat *words_stat);
  void close();
  void open(std::string output_file);
  bool write(const std::vector<const std::string *> &order);
};
