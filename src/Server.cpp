#include <iostream>
#include <string>
#include <cctype>
using namespace std;

bool match_group(const string& input_line, const string& pattern) {
    return input_line.find_first_of(pattern) != string::npos;
    // returns the position of the first occurrence of any character that is present in the argument string
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.size() == 0) return true;
    if (input_line.size() == 0) return false;

    if (pattern.substr(0, 2) == "\\d") {
        if (isdigit(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2));
        }
        return false; // Return false if the current character is not a digit
    } else if (pattern.substr(0, 2) == "\\w") {
        if (isalnum(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2));
        }
        return false; // Return false if the current character is not alphanumeric
    } else if (pattern[0] == '[') {
        auto first = pattern.find(']');
        bool neg = pattern[1] == '^';
        if (neg) {
            if (!match_group(input_line, pattern.substr(2, first - 2))) {
                return match_pattern(input_line.substr(1), pattern.substr(first + 1));
            }
            return false;
        }
        if (match_group(input_line, pattern.substr(1, first - 1))) {
            return match_pattern(input_line.substr(1), pattern.substr(first + 1));
        } else {
            return false;
        }
    }

    if (pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1));
    }
    return false;
}

bool match_patterns(const std::string& input_line, const std::string& pattern) {
    size_t i = 0;
    while (i < input_line.size()) {
        if (match_pattern(input_line.substr(i), pattern)) {
            return true;
        }
        i++;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./your_program -E <pattern>" << endl;
        return 1;
    }

    string flag = argv[1];
    string pattern = argv[2];

    if (flag != "-E") {
        cerr << "Expected '-E' flag" << endl;
        return 1;
    }

    string input_line;
    getline(cin, input_line);

    try {
        if (match_patterns(input_line, pattern)) {
            return 0;  // Pattern matched
        } else {
            return 1;  // Pattern did not match
        }
    } catch (const std::runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
