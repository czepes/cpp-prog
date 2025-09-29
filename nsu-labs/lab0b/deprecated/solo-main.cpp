#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>

using WordsFreq = std::pair<int, double>;
using WordsData = std::pair<std::string, WordsFreq>;

class WordsStat {
private:
  std::ifstream input;
  std::vector<WordsData> data;

  std::string format_line(std::string line) {
    for (char &ch : line) {
      if (ispunct(ch)) {
        ch = ' ';
      } else if (isupper(ch)) {
        ch = tolower(ch);
      }
    }

    return line;
  }

public:
  WordsStat(const std::string input_file) {
    input.open(input_file);
  }

  ~WordsStat() {
    if (input.is_open()) {
      input.close();
    }
  }
  
  const std::vector<WordsData>& get_data() {
    return data;
  }

  void close() {
    if (input.is_open()) {
      input.close();
    }
  }

  void open(std::string input_file) {
    this->close();
    input.open(input_file);
  }

  void count_words() {
    if (!input.is_open()) {
      std::cerr << "Error: Input file is not open" << std::endl;
      return;
    }

    std::map<std::string, int> words_map;
    std::string line;
    std::string word;
    int total;

    while (std::getline(input, line)) {
      std::stringstream words(format_line(line));

      while (words >> word) {
        words_map[word]++;
        total++;
      }
    }

    for (const auto& [word, amount] : words_map) {
      data.push_back(
        std::pair(word, std::pair(amount, (double) amount / total))
      );
    }

    std::sort(
      data.begin(), data.end(),
      [](
        const WordsData a,
        const WordsData b
      ) {
        return a.second.first > b.second.first;
      }
    );
  }
};

class Writer {
private:
  std::ofstream output;
  WordsStat* stat;

public:
  ~Writer() {
    if (output.is_open()) {
      output.close();
    }
  }

  Writer(std::string output_file, WordsStat* words_stat) {
    output.open(output_file);
    stat = words_stat;
  }

  void close() {
    if (output.is_open()) {
      output.close();
    }
  }

  void open(std::string output_file) {
    this->close();
    output.open(output_file);
  }

  bool write() {
    if (!output.is_open()) {
      std::cerr << "Error: Output file is not open" << std::endl;
      return false;
    }

    output << "word;amount;rate(%)" << std::endl;

    for (const auto& [word, word_stat] : stat->get_data()) {
      output << '"' << word << '"' << ";" << word_stat.first << ";"
        << word_stat.second << std::endl;
    }

    return true;
  }
};

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Wrong amount of arguments" << std::endl;
  }

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);

  WordsStat stat(argv[1]);
  stat.count_words();

  Writer writer(argv[2], &stat);
  writer.write();

  return EXIT_SUCCESS;
}
