#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct ParsedCommand {
  string command;
  vector<string> params;
  int line_num;

  ParsedCommand() : command(""), params({}), line_num(0) {};
  ParsedCommand(const string &command, const vector<string> &params,
                int line_num)
      : command(command), params(params), line_num(line_num) {};

  bool is_comment() const { return command.empty() || command[0] == '#'; }
};

class ConfigParser {
private:
  ifstream stream;
  const vector<string> &files;
  int line_num = 0;

  string trim(const string &str) const;
  vector<string> split_line(const string &line) const;
  bool is_comment_line(const string &line) const;
  bool is_empty_line(const string &line) const;

public:
  ConfigParser(const string &config_file, const vector<string> &files);
  ~ConfigParser();

  bool read_next_command(ParsedCommand &parsed_command);

  int get_line_num() const { return line_num; }
  bool eof() const { return stream.eof(); }

  string parse_file_ref(const string &param, int line_num);

  static double parse_time_param(const string &param, double max_value,
                                 int line_num, bool end = false);
  static double parse_factor_param(const string &param, double min_value = 0,
                                   double max_value = -1, int line_num = 0);
};

#endif
