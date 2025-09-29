#include "words-stat.h"
#include "words-writer.h"
#include <algorithm>
#include <iostream>

void help() {
  std::cout << "Count words frequency in [input-file] "
            << "and write result in [output-file] in CSV format" << std::endl;
  std::cout << "Arguments:" << std::endl;
  std::cout << "  [input-file] - path to input file" << std::endl;
  std::cout << "  [output-file] - path to output file" << std::endl;
}

void sort_words_by_freq(WordsStat &stat,
                        std::vector<const std::string *> &sorted) {
  sorted.reserve(stat.get_total());

  for (const auto &pair : stat.get_freq_map()) {
    sorted.emplace_back(&pair.first);
  }

  std::sort(sorted.begin(), sorted.end(),
            [&stat](const std::string *w1, const std::string *w2) {
              return stat.get_freq(*w1).first > stat.get_freq(*w2).first;
            });
}

int main(int argc, char **argv) {
  if (argc != 3) {
    help();
    std::cerr << "Wrong amount of arguments" << std::endl;
    return EXIT_FAILURE;
  }

  WordsStat stat(argv[1]);
  stat.count_words();

  std::vector<const std::string *> sorted;
  sort_words_by_freq(stat, sorted);

  WordsWriter writer(argv[2], &stat);
  writer.write(sorted);

  return EXIT_SUCCESS;
}
