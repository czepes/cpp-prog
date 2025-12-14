#include "config-parser.h"
#include "errors.h"
#include <exception>
#include <sstream>
#include <string>

ConfigParser::ConfigParser(const string &config_file,
                           const vector<string> &files)
    : files(files), line_num(0) {
  stream.open(config_file);
  if (!stream.is_open()) {
    throw ConfigParserError("Failed to open config file " + config_file);
  }
}

ConfigParser::~ConfigParser() {
  if (stream.is_open()) {
    stream.close();
  }
}

string ConfigParser::trim(const string &str) const {
  size_t first = str.find_first_not_of(" \t");
  if (first == string::npos) {
    return "";
  }
  size_t last = str.find_last_not_of(" \t");
  return str.substr(first, last - first + 1);
}

vector<string> ConfigParser::split_line(const string &line) const {
  vector<string> tokens;
  stringstream ss(line);
  string token;

  while (ss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

bool ConfigParser::is_comment_line(const string &line) const {
  string trimmed = trim(line);
  return !trimmed.empty() && trimmed[0] == '#';
}

bool ConfigParser::is_empty_line(const string &line) const {
  string trimmed = trim(line);
  return trimmed.empty();
}

bool ConfigParser::read_next_command(ParsedCommand &parsed_command) {
  string line;

  while (getline(stream, line)) {
    line_num++;

    if (is_empty_line(line) || is_comment_line(line)) {
      continue;
    }

    vector<string> tokens = split_line(line);
    if (tokens.empty()) {
      continue;
    }

    string command = tokens[0];
    vector<string> params(tokens.begin() + 1, tokens.end());
    for (auto &param : params) {
      if (param[0] == '$') {
        param = parse_file_ref(param, line_num);
      }
    }

    parsed_command = ParsedCommand(command, params, line_num);
    return true;
  }

  return false;
}

string ConfigParser::parse_file_ref(const string &param, int line_num) {
  if (param.empty() || param[0] != '$') {
    throw ConfigParserError(line_num,
                            "Expected file reference to start with '$'");
  }

  int index;
  int max_index = files.size();

  try {
    index = stoi(param.substr(1));
  } catch (const exception &e) {
    throw ConfigParserError(line_num, "Invalid file reference'" + param +
                                          "': " + e.what());
  }

  if (index < 1 || index > max_index) {
    throw ConfigParserError(line_num,
                            "File index '" + to_string(index) +
                                "' out of range. Expected from 1 to " +
                                to_string(max_index));
  }

  return files[index];
}

double ConfigParser::parse_time_param(const string &param, double max_value,
                                      int line_num, bool end) {
  double time;

  try {
    time = stod(param);
  } catch (const exception &e) {
    throw ConfigParserError(line_num, "Invalid time parameter '" + param +
                                          "': " + e.what());
  }

  if (time < 0) {
    if (!end && time != -1.0) {
      throw ConfigParserError(line_num, "Time '" + to_string(time) +
                                            "' cannot be negative");
    }
  } else if (time > max_value) {
    throw ConfigParserError(line_num, "Time '" + to_string(time) +
                                          "' exceeds maximum value " +
                                          to_string(max_value));
  }

  return time;
}

double ConfigParser::parse_factor_param(const string &param, double min_value,
                                        double max_value, int line_num) {
  double factor;

  try {
    factor = stod(param);
  } catch (const exception &e) {
    throw ConfigParserError(line_num, "Invalid factor parameter: '" + param +
                                          "': " + e.what());
  }

  if (factor < min_value || factor > max_value) {
    throw ConfigParserError(line_num, "Factor value '" + to_string(factor) +
                                          "' out of range. Expected from " +
                                          to_string(min_value) + " to " +
                                          to_string(max_value));
  }

  return factor;
}
