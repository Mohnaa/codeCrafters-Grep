#include <iostream>
#include <string>
#include <stack>
#include <cctype>

// Match if the input contains any digit
bool match_digit(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isdigit(ch)) return true;
    }
    return false;
}

// Match if the input contains any alphanumeric character
bool match_alphanumeric(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isalnum(ch)) return true;
    }
    return false;
}

// Match a positive group (e.g., [a-z])
bool positive_match_group(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.size()) end = pattern.size();
    int idx = start, pattern_size = end;

    // Parse the group and store individual characters or ranges
    while (idx < pattern_size) {
        if (idx != pattern_size - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({temp, pattern[idx]});
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }

    // Check for individual character matches
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return true;
    }

    // Check for character range matches
    while (!s_pair.empty()) {
        std::pair<char, char> temp = s_pair.top();
        s_pair.pop();
        char start_char = temp.first, end_char = temp.second;
        for (char ch = start_char; ch <= end_char; ++ch) {
            if (input_line.find(ch) != std::string::npos) return true;
        }
    }

    return false;
}

// Match a negative group (e.g., [^a-z])
bool negative_match_group(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.length()) end = pattern.length();
    int idx = start + 1, pattern_size = end;

    // Parse the group and store individual characters or ranges
    while (idx < pattern_size - 1) {
        if (idx != pattern_size - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({temp, pattern[idx]});
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }

    // Ensure none of the individual characters are in the input
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return false;
    }

    // Ensure none of the character ranges are in the input
    while (!s_pair.empty()) {
        std::pair<char, char> temp = s_pair.top();
        s_pair.pop();
        char start_char = temp.first, end_char = temp.second;
        for (char ch = start_char; ch <= end_char; ++ch) {
            if (input_line.find(ch) != std::string::npos) return false;
        }
    }

    return true;
}

// Match input against a complex pattern
bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    while (i < input_line.size()) {
        int j = 0;
        bool start_anchor = false;
        if (pattern[0] == '^') {
            j++;
            start_anchor = true;
        }
        int temp = i;
        while (j < pattern.size() && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                j++;
                if (j < pattern.size()) {
                    if (pattern[j] == 'd') {
                        if (!std::isdigit(input_line[temp])) break;
                        else temp++;
                    } else if (pattern[j] == 'w') {
                        if (!std::isalnum(input_line[temp])) break;
                        else temp++;
                    }
                }
            } else if (pattern[j] == '[') {
                int start = j;
                while (j < pattern.size() && pattern[j] != ']') j++;
                if (pattern[start + 1] == '^') {
                    if (!negative_match_group(input_line.substr(temp), pattern, start, j)) break;
                    else temp++;
                } else {
                    if (!positive_match_group(input_line.substr(temp), pattern, start, j)) break;
                    else temp++;
                }
            } else {
                if (input_line[temp] != pattern[j]) break;
                temp++;
            }
            j++;
        }

        if (j == pattern.size()) return true;
        if (start_anchor && j != pattern.size()) return false;
        i++;
    }

    return false;
}

// Match input against the given pattern
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '\\' && pattern.length() == 2) {
        switch (pattern[1]) {
        case 'd':
            return match_digit(input_line);
        case 'w':
            return match_alphanumeric(input_line);
        default:
            throw std::runtime_error("Unhandled pattern " + pattern);
        }
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negative_match_group(input_line, pattern, 0, pattern.size());
        } else {
            return positive_match_group(input_line, pattern, 0, pattern.size());
        }
    } else if (pattern.length() > 1) {
        return match(input_line, pattern);
    } else {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
}

int main(int argc, char* argv[]) {
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
