#include <iostream>
#include <string>
#include <cctype>
#include <unordered_set>

// Function to handle alternation (either/or) like (cat|dog)
bool alternate(const std::string& regex, const std::string& text) {
    size_t close_bracket_pos = regex.find(')');
    size_t pipe_pos = regex.find('|');
    if (close_bracket_pos == std::string::npos || pipe_pos == std::string::npos) {
        throw std::runtime_error("Invalid regex: missing '|' or ')'");
    }

    // Extract alternates without spaces around the alternation
    std::string alter1 = regex.substr(0, pipe_pos);
    std::string alter2 = regex.substr(pipe_pos + 1, close_bracket_pos - pipe_pos - 1);

    // Try matching either alternative
    return match_here(alter1, text) || match_here(alter2, text);
}

bool match_here(const std::string& regex, const std::string& text) {
    if (regex.empty()) return text.empty();

    // Handle alternation group (e.g., (cat|dog))
    if (regex[0] == '(') {
        size_t close_bracket_pos = regex.find(')');
        if (close_bracket_pos == std::string::npos) {
            throw std::runtime_error("Invalid regex: unmatched '('");
        }
        return alternate(regex.substr(1, close_bracket_pos - 1), text);
    }

    // Literal character match (including spaces)
    if (!text.empty() && regex[0] == text[0]) {
        return match_here(regex.substr(1), text.substr(1));
    }

    return false; // No match
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    return match_here(pattern, input_line);
}

int main(int argc, char* argv[]) {
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
