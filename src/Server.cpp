#include <iostream>
#include <string>
#include <stack>

// Check if input_line contains any digit
bool matchdigit(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isdigit(ch)) return true;
    }
    return false;
}

// Check if input_line contains any alphanumeric character
bool matchalphanumeric(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isalnum(ch)) return true;
    }
    return false;
}

// Positive match group function
bool positiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.size()) end = pattern.size();
    int idx = start, patternSize = end;

    while (idx < patternSize) {
        if (idx != patternSize - 1 && pattern[idx] == '-') {
            idx++;
            // Logic to handle range match (e.g., 'a-z') can be added here
        }
        // Additional logic can be added here
        idx++;
    }
    return false;
}

// Negative match group function
bool negitiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.size()) end = pattern.size();
    int idx = start + 1, patternSize = end;

    while (idx < patternSize - 1) {
        if (idx != patternSize - 1 && pattern[idx] == '-') {
            idx++;
            // Logic to handle range match can be added here
        }
        // Additional logic can be added here
        idx++;
    }
    return true;
}

// General pattern matching function
bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;

    while (i < input_line.size()) {
        int j = 0;
        int temp = i;

        while (j < pattern.size() && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                j++;
                if (j < pattern.size()) {
                    if (pattern[j] == 'd') { // digit
                        if (!isdigit(input_line[temp])) break;
                        else temp++;
                    } else if (pattern[j] == 'w') { // alphanumeric
                        if (!isalnum(input_line[temp])) break;
                        else temp++;
                    } else if (pattern[j] == '[') { // character class
                        int start = j;
                        while (j < pattern.size() && pattern[j] != ']') j++;

                        if (pattern[start + 1] == '^') {
                            return negitiveMatchGroup(input_line, pattern, start, j + 1);
                        } else {
                            return positiveMatchGroup(input_line, pattern, start, j + 1);
                        }
                    }
                } else {
                    break;
                }
            } else {
                if (input_line[temp] != pattern[j]) break;
                else temp++;
            }
            j++;
        }
        if (j == pattern.size()) return true;
        i++;
    }

    return false;
}

// Top-level pattern matching function
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negitiveMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    } else if (pattern.length() > 1) {
        return match(input_line, pattern);
    } else {
        throw std::runtime_error("Unhandled pattern: " + pattern);
    }
}

int main(int argc, char* argv[]) {
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
