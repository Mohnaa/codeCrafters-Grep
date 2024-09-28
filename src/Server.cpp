#include <iostream>
#include <string>
#include <cctype>
#include <unordered_set>
bool match_set(char c, const std::string& set, bool negate);
bool match_here(const std::string& regex, const std::string& text);
bool match_digit(const char c) {
	return std::isdigit(c);
}
bool match_alphanumeric(const char c) {
	return std::isalnum(c);
}
bool match_group(const std::string& regex, const std::string& text) {
	if (text.empty()) return false;
	bool negate = regex[0] == '^';
	size_t close_bracket_pos = regex.find(']');
	if (close_bracket_pos == std::string::npos) {
		throw std::runtime_error("Invalid regex");
	}
	size_t start = negate ? 1 : 0;
	if (match_set(text[0], regex.substr(start, close_bracket_pos - start), negate)) {
		return match_here(regex.substr(close_bracket_pos + 1), text.substr(1));
	}
	return false;
}
bool match_set(char c, const std::string& set, bool negate = false) {
	bool found = set.find(c) != std::string::npos;
	return negate ? !found : found;
}
bool alternate(const std::string& regex, const std::string& text) {
    size_t clost_bracket_pos = regex.find(')');
    size_t pipe_pos = regex.find('|');
    if (clost_bracket_pos == std::string::npos || pipe_pos == std::string::npos) {
        throw std::runtime_error("Invalid regex");
    }
    std::string alter1 = regex.substr(0, pipe_pos - 1);
    std::string alter2 = regex.substr(pipe_pos + 1, clost_bracket_pos - pipe_pos - 1);
    return match_here(alter1, text) || match_here(alter2, text);
}
bool match_here(const std::string& regex, const std::string& text) {
    if (text.empty()) {
        return regex.empty() || (regex.size() == 2 && regex[1] == '?');

        if (regex[0] == '[' && match_group(regex.substr(1), std::string(1, text[0]))) {
            return true;
        }
        size_t i = 0;
        if (regex[0] == '(' && alternate(regex.substr(1), text.substr(i))) {
            return true;
        }
    }
    return false;
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