#include "WordBreaker.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

std::string WordBreaker::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string WordBreaker::lowerCase(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

std::vector<std::string> WordBreaker::splitString(const std::string& line, char delimiter) {
    std::vector<std::string> result;
    std::string item;
    std::stringstream ss(line);
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    return result;
}

std::vector<std::string> WordBreaker::splitCSV(const std::string& line) {
    std::vector<std::string> raw = splitString(line, ',');
    std::vector<std::string> result;
    for (const std::string& part : raw) {
        std::string cleaned = lowerCase(trim(part));
        if (!cleaned.empty()) result.push_back(cleaned);
    }
    return result;
}

std::string WordBreaker::joinCSV(const std::vector<std::string>& items) {
    std::string result;
    for (size_t i = 0; i < items.size(); ++i) {
        result += items[i];
        if (i + 1 < items.size()) result += ',';
    }
    return result;
}

void WordBreaker::addLinkingElements() {
    for (const std::string& e : {"s", "es", "n", "en", "er", "e"}) {
        dict[e] = {"linking element", {}};
    }
}

bool WordBreaker::loadDictionary(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> fields = splitString(line, '|');
        if (fields.size() < 2) continue;

        std::string word = lowerCase(trim(fields[0]));
        std::string meaning = trim(fields[1]);
        std::vector<std::string> subparts;
        if (fields.size() >= 3) subparts = splitCSV(fields[2]);

        if (!word.empty()) dict[word] = {meaning, subparts};
    }
    return true;
}

bool WordBreaker::saveEntryToFile(const std::string& filename, const std::string& word) const {
    auto it = dict.find(lowerCase(trim(word)));
    if (it == dict.end()) return false;

    std::ofstream file(filename, std::ios::app);
    if (!file) return false;

    file << it->first << '|' << it->second.meaning << '|'
         << joinCSV(it->second.subparts) << '\n';
    return true;
}

bool WordBreaker::addEntry(const std::string& word,
                           const std::string& meaning,
                           const std::vector<std::string>& subparts) {
    std::string cleanWord = lowerCase(trim(word));
    if (cleanWord.empty()) return false;
    dict[cleanWord] = {trim(meaning), subparts};
    return true;
}

bool WordBreaker::contains(const std::string& word) const {
    return dict.count(lowerCase(trim(word))) > 0;
}

bool WordBreaker::segmentHelper(const std::string& word,
                                size_t start,
                                std::vector<std::string>& parts,
                                std::unordered_map<size_t, bool>& memo) const {
    if (start == word.size()) return true;

    auto memoIt = memo.find(start);
    if (memoIt != memo.end() && !memoIt->second) return false;

    for (size_t len = word.size() - start; len >= 1; --len) {
        std::string piece = word.substr(start, len);
        if (dict.count(piece)) {
            parts.push_back(piece);
            if (segmentHelper(word, start + len, parts, memo)) {
                memo[start] = true;
                return true;
            }
            parts.pop_back();
        }
    }

    memo[start] = false;
    return false;
}

std::vector<std::string> WordBreaker::greedySplit(const std::string& word) const {
    std::vector<std::string> parts;
    size_t i = 0;

    while (i < word.size()) {
        std::string best;
        for (size_t len = word.size() - i; len >= 1; --len) {
            std::string piece = word.substr(i, len);
            if (dict.count(piece)) {
                best = piece;
                break;
            }
        }

        if (!best.empty()) {
            parts.push_back(best);
            i += best.size();
        } else {
            parts.push_back(std::string(1, word[i]));
            ++i;
        }
    }

    return parts;
}

std::vector<std::string> WordBreaker::splitUnknownCompound(const std::string& word) const {
    std::string cleanWord = lowerCase(trim(word));
    std::vector<std::string> parts;
    std::unordered_map<size_t, bool> memo;

    if (segmentHelper(cleanWord, 0, parts, memo)) return parts;
    return greedySplit(cleanWord);
}

AnalysisNode WordBreaker::buildTree(const std::string& word,
                                    std::unordered_set<std::string>& visited) const {
    AnalysisNode node;
    node.word = word;

    if (visited.count(word)) {
        node.known = true;
        node.cycle = true;
        node.meaning = "cycle detected";
        return node;
    }

    auto it = dict.find(word);
    if (it == dict.end()) {
        node.known = false;
        node.meaning = "unknown";
        return node;
    }

    node.known = true;
    node.meaning = it->second.meaning;
    visited.insert(word);

    for (const std::string& sub : it->second.subparts) {
        node.children.push_back(buildTree(sub, visited));
    }

    return node;
}

AnalysisNode WordBreaker::analyze(const std::string& input) const {
    std::string cleanInput = lowerCase(trim(input));
    std::unordered_set<std::string> visited;

    if (contains(cleanInput)) {
        return buildTree(cleanInput, visited);
    }

    AnalysisNode root;
    root.word = cleanInput;
    root.meaning = "unknown full compound";
    root.known = false;

    for (const std::string& part : splitUnknownCompound(cleanInput)) {
        root.children.push_back(buildTree(part, visited));
    }

    return root;
}
