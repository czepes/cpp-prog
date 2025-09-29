#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cctype>
#include <utility>
#include <algorithm>

int count_words(
  std::ifstream& input,
  std::map<std::string, int>& words_map
) {
  if (!input.is_open()) {
    std::cerr << "Error: Input file is not open" << std::endl;
    return EXIT_FAILURE;
  }

  std::string line;
  std::string word;
  int count = 0;

  while (std::getline(input, line)) {

    for (char &ch : line) {
      if (ispunct(ch)) {
        ch = ' ';
      } else if (isupper(ch)) {
        ch = tolower(ch);
      }
    }

    std::stringstream words(line);

    while (words >> word) {
      words_map[word]++;
      count++;
    }
  }

  return count;
}

int write_words(
  std::ofstream& output,
  std::map<std::string, int>& words_map,
  int total
) {
  if (!output.is_open()) {
    std::cerr << "Error: Output file is not open" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::pair<std::string, int>> sorted;

  for (const auto& pair : words_map) {
    sorted.push_back(pair);
  }

  std::sort(
    sorted.begin(), sorted.end(),
    [](
      const std::pair<std::string, int> a,
      const std::pair<std::string, int> b
    ) {
      return a.second > b.second;
    }
  );

  output << "word;amount;rate(%)" << std::endl;

  for (const auto& [word, amount] : sorted) {
    output << '"' << word << '"' << ";" << amount << ";"
      << (double) amount / total << std::endl;
  }

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Wrong amount of arguments" << std::endl;
  }

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2]);

  std::map<std::string, int> words_map;

  int counted = count_words(input, words_map);

  if (counted < 0) {
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  int status = write_words(output, words_map, counted);

  if (status == EXIT_FAILURE) {
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  input.close();
  output.close();
  return EXIT_SUCCESS;
}
