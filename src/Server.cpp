#include <iostream>
#include <string>
bool match_pattern(const std::string &input_line, const std::string &pattern) {
  if (pattern.length() == 1) {
    return input_line.find(pattern) != std::string::npos;
  } else if (pattern == "\\d") {
    for (char c : input_line) {
      if (std::isdigit(c)) {
        return true;
      }
    }
    return false;
  } else if (pattern == "\\w") {
    for (char c : input_line) {
      if (std::isalpha(c)) {
        return true;
      }
    }
    return false;
  } else if (pattern[0] == '[' && pattern[pattern.size() - 1] == ']') {
    for (char c : pattern) {
      for (char d : input_line) {
        if (c == d) {
          return true;
        }
    bool negate = pattern[1] == '^';
    std::string chars_to_match =
        pattern.substr(negate ? 2 : 1, pattern.size() - (negate ? 3 : 2));
    for (const auto &c : input_line) {
      if (chars_to_match.find(c) != std::string::npos && !negate) {
        return true;
      } else if (chars_to_match.find(c) == std::string::npos && negate) {
        return true;
      }
    }
    return false;
  } else {
    throw std::runtime_error("Unhandled pattern " + pattern);
  }
}


int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    // Uncomment this block to pass the first stage
    //
    std::string input_line;
    std::getline(std::cin, input_line);
    
    try {
        if (match_pattern(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
