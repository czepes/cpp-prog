#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

class CSVError : public runtime_error {
private:
  size_t line_num;

public:
  CSVError(size_t line_num, const string &msg)
      : runtime_error("Line " + to_string(line_num) + ": " + msg),
        line_num(line_num) {};

  size_t get_line_number() const { return line_num; };
};

template <typename... Types> class CSVParser {
private:
  ifstream *in;
  size_t skip_lines;
  char delimiter = ';';
  char quote = '"';
  bool owns_stream = false;

public:
  CSVParser(string path, size_t skip_lines = 0, char delimiter = ';',
            char quote = '"')
      : skip_lines(skip_lines), delimiter(delimiter), quote(quote),
        owns_stream(true) {
    in = new ifstream(path);
    if (!in) {
      delete in;
      throw runtime_error("Failed to open input file '" + path +
                          "' for CSVParser");
    }
  }

  CSVParser(ifstream &in, size_t skip_lines = 0, char delimiter = ';',
            char quote = '"')
      : in(&in), skip_lines(skip_lines), delimiter(delimiter), quote(quote) {
    if (!in) {
      throw runtime_error("Invalid input stream for CSVParser");
    }
  }

  ~CSVParser() {
    if (owns_stream) {
      if (in && in->is_open()) {
        in->close();
      }
      delete in;
    }
  }

  void set_delimiter(char delimiter) { this->delimiter = delimiter; }
  void set_quote(char quote) { this->quote = quote; }

  class iterator;

  iterator begin() {
    in->clear();
    in->seekg(0);
    return iterator(this);
  };
  iterator end() { return iterator(); };

  class iterator {
  private:
    CSVParser *parser;
    tuple<Types...> current;
    size_t line_num = 0;
    bool is_end = false;

  public:
    iterator() : parser(nullptr), is_end(true) {}
    iterator(CSVParser *parser) : parser(parser) {
      for (size_t i = 0; i < parser->skip_lines; i++) {
        string _;
        if (!getline(*parser->in, _)) {
          is_end = true;
          return;
        }
      }
      line_num += parser->skip_lines;
      is_end = !read_line();
    }

    iterator operator++() {
      is_end = !read_line();
      return *this;
    }

    iterator operator++(int) {
      iterator temp = *this;
      ++(*this);
      return temp;
    }

    const tuple<Types...> operator*() const { return current; }

    bool operator!=(const iterator &other) const {
      return is_end != other.is_end;
    }
    bool operator==(const iterator &other) const {
      return is_end == other.is_end;
    }

  private:
    bool read_line() {
      if (!parser || !parser->in || parser->in->eof()) {
        return false;
      }

      string line;
      vector<string> cells;

      while (getline(*parser->in, line)) {
        line_num++;

        if (line.empty()) {
          continue;
        }

        cells = parser->parse_line(line, line_num);

        if (cells.size() != sizeof...(Types)) {
          throw CSVError(line_num, "Expected " + to_string(sizeof...(Types)) +
                                       " columns, but got " +
                                       to_string(cells.size()));
        }

        current = convert_cells<Types...>(cells);
        return true;
      }

      return false;
    }

    template <typename... Args>
    tuple<Args...> convert_cells(const vector<string> &cells) {
      return convert_cells<Args...>(cells, index_sequence_for<Args...>{});
    }

    template <typename... Args, size_t... Is>
    tuple<Args...> convert_cells(const vector<string> &cells,
                                 index_sequence<Is...>) {
      return make_tuple<Args...>(convert<Args>(cells[Is], line_num, Is + 1)...);
    }

    template <typename Type>
    Type convert(const string &str, size_t line_num, size_t col_num) {
      try {
        if constexpr (is_same_v<Type, string>) {
          return str;
        } else if constexpr (is_same_v<Type, int>) {
          return stoi(str);
        } else if constexpr (is_same_v<Type, long>) {
          return stol(str);
        } else if constexpr (is_same_v<Type, float>) {
          return stof(str);
        } else if constexpr (is_same_v<Type, double>) {
          return stod(str);
        } else {
          stringstream converter(str);
          Type value;
          converter >> value;
          if (converter.fail()) {
            throw invalid_argument("Conversion via stringstream failed");
          }
          return value;
        }
      } catch (const exception &e) {
        throw CSVError(line_num, "Column " + to_string(col_num) +
                                     ": Failed to convert '" + str +
                                     "': " + e.what());
      }
    }
  };

private:
  vector<string> parse_line(const string &line, size_t line_num) {
    vector<string> cells;

    if (line.empty()) {
      return cells;
    }

    bool inside_quotes = false;
    string cell;
    for (size_t i = 0; i < line.length(); i++) {
      char c = line[i];

      if (c == quote) {
        if (!inside_quotes && !cell.empty()) {
          throw CSVError(line_num,
                         "Quote character at the middle of column at " +
                             to_string(i + 1));
        }
        if (inside_quotes && i + 1 < line.length() && line[i + 1] == quote) {
          cell += c;
          i++;
        } else {
          inside_quotes = !inside_quotes;
        }
      } else if (!inside_quotes && c == delimiter) {
        cells.push_back(cell);
        cell.clear();
      } else {
        cell += c;
      }
    }

    if (inside_quotes) {
      throw CSVError(line_num, "Quotes are not closed at the end of the line");
    }

    cells.push_back(cell);

    return cells;
  }
};

#endif
