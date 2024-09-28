#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Helper to decompose the pattern into parts
std::vector<std::string> decompose_pattern(const std::string& pattern) {
    std::vector<std::string> res;
    const size_t n = pattern.length();
    const std::string quantifiers("+?");
    size_t i = 0;
    while (i < n) {
        size_t matched = 0;
        if (pattern[i] == '\\' && i+1 < n) {
            if (i+2 < n && quantifiers.find(pattern[i+2]) != std::string::npos) {
                matched = 3;
            } else {
                matched = 2;
            }
        } else if (pattern[i] == '(' && i+2 < n) {
            matched = pattern.find_first_of(')', i + 1) - i + 1;
        } else if (pattern[i] == '[' && i+2 < n) {
            const size_t k = pattern.find_first_of(']', i + 1);
            if (k+1 < n && quantifiers.find(pattern[k+1]) != std::string::npos) {
                matched = k - i + 2;
            } else {
                matched = k - i + 1;
            }
        } else {
            if (i+1 < n && quantifiers.find(pattern[i+1]) != std::string::npos) {
                matched = 2;
            } else {
                matched = 1;
            }
        }
        res.push_back(pattern.substr(i, matched));
        i += matched;
    }
    return res;
}

// Helper to check if a character matches a pattern
bool char_matches_pattern(const char c, const std::string& pattern) {
    if (pattern == ".") {
        return true;
    } else if (pattern.length() == 1) {
        return c == pattern[0];
    } else if (pattern == "\\d") {
        return std::isdigit(static_cast<unsigned char>(c));
    } else if (pattern == "\\w") {
        return std::isalnum(static_cast<unsigned char>(c));
    } else if (pattern.find("[^") == 0 && pattern.rfind(']') == pattern.length() - 1) {
        const std::string chars(pattern.cbegin() + 2, pattern.cend() - 1);
        return chars.find(c) == std::string::npos;
    } else if (pattern.find('[') == 0 && pattern.rfind(']') == pattern.length() - 1) {
        const std::string chars(pattern.cbegin() + 1, pattern.cend() - 1);
        return chars.find(c) != std::string::npos;
    } else {
        throw std::runtime_error("Unhandled pattern: " + pattern);
    }
}

// Matching function at the beginning of the input string
size_t match_pattern_at_beginning(const std::string& input_line, 
                                  const std::vector<std::string>& patterns,
                                  std::vector<std::string>& captures) {
    const size_t n = patterns.size();
    const size_t m = input_line.length();
    size_t pos = 0;
    for (size_t i = 0; i < n; ++i) {
        if (patterns[i] == "$") {
            return pos == m ? m : 0;
        }
        if (patterns[i].find('(') == 0 && patterns[i].rfind(')') == patterns[i].length() - 1) {
            const auto p = patterns[i].find_first_of('|');
            if (p == std::string::npos) {
                const auto pattern = decompose_pattern(patterns[i].substr(1, patterns[i].length() - 2));
                const auto sz = match_pattern_at_beginning(input_line.substr(pos), pattern, captures);
                if (sz == 0) {
                    return 0;
                } else {
                    captures.emplace_back(input_line.substr(pos, sz));
                    pos += sz;
                }
            } else {
                const auto pattern1 = decompose_pattern(patterns[i].substr(1, p - 1));
                const auto pattern2 = decompose_pattern(patterns[i].substr(p + 1, patterns[i].length() - p - 2));
                const auto sz1 = match_pattern_at_beginning(input_line.substr(pos), pattern1, captures);
                const auto sz2 = match_pattern_at_beginning(input_line.substr(pos), pattern2, captures);
                if (sz1 > 0) {
                    captures.emplace_back(input_line.substr(pos, sz1));
                    pos += sz1;
                } else if (sz2 > 0) {
                    captures.emplace_back(input_line.substr(pos, sz2));
                    pos += sz2;
                } else {
                    return 0;
                }
            }
        } else if (!patterns[i].empty() && patterns[i].back() == '+') {
            const std::string pattern = patterns[i].substr(0, patterns[i].length() - 1);
            if (pos >= m || !char_matches_pattern(input_line[pos], pattern)) {
                return 0;
            }
            ++pos;
            while (pos < m && char_matches_pattern(input_line[pos], pattern)) {
                ++pos;
            }
        } else if (patterns[i].size() > 0 && patterns[i].back() == '?') {
            const std::string pattern = patterns[i].substr(0, patterns[i].length() - 1);
            if (pos < m && char_matches_pattern(input_line[pos], pattern)) {
                ++pos;
            }
        } else if (patterns[i].length() == 2 && patterns[i][0] == '\\' && std::isdigit(static_cast<unsigned char>(patterns[i][1]))) {
            size_t k = std::stoul(patterns[i].substr(1));
            if (captures.size() < k) {
                throw std::runtime_error("Invalid backreference: " + patterns[i]);
            }
            const auto& group = captures[k - 1];
            if (input_line.substr(pos, group.length()) != group) {
                return 0;
            }
            pos += group.length();
        } else if (pos >= m || !char_matches_pattern(input_line[pos], patterns[i])) {
            return 0;
        } else {
            ++pos;
        }
    }
    return pos;
}

// Wrapper to match patterns from the start
bool match_pattern_at_beginning(const std::string& input_line, 
                                const std::vector<std::string>& patterns) {
    std::vector<std::string> captures;
    return match_pattern_at_beginning(input_line, patterns, captures) > 0;
}

// Main pattern matching logic
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    const auto patterns = decompose_pattern(pattern);
    if (patterns.empty()) throw std::runtime_error("Unhandled pattern: " + pattern);
    if (patterns[0] == "^") {
        return match_pattern_at_beginning(input_line, {patterns.cbegin() + 1, patterns.cend()});
    }
    for (size_t i = 0; i < input_line.length(); ++i) {
        if (match_pattern_at_beginning(input_line.substr(i), patterns)) {
            return true;
        }
    }
    return false;
}

// Main program entry
int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
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
