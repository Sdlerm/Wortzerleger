#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Entry {
    std::string meaning;
    std::vector<std::string> subparts;
};

struct AnalysisNode {
    std::string word;
    std::string meaning;
    bool known = false;
    bool cycle = false;
    std::vector<AnalysisNode> children;
};

class WordBreaker {
public:
    void addLinkingElements();
    bool loadDictionary(const std::string& filename);
    bool saveEntryToFile(const std::string& filename, const std::string& word) const;
    bool addEntry(const std::string& word,
                  const std::string& meaning,
                  const std::vector<std::string>& subparts);

    bool contains(const std::string& word) const;
    std::vector<std::string> splitUnknownCompound(const std::string& word) const;
    AnalysisNode analyze(const std::string& input) const;

    static std::string trim(const std::string& s);
    static std::string lowerCase(std::string s);
    static std::vector<std::string> splitString(const std::string& line, char delimiter);
    static std::vector<std::string> splitCSV(const std::string& line);
    static std::string joinCSV(const std::vector<std::string>& items);

private:
    std::unordered_map<std::string, Entry> dict;

    bool segmentHelper(const std::string& word,
                       size_t start,
                       std::vector<std::string>& parts,
                       std::unordered_map<size_t, bool>& memo) const;

    std::vector<std::string> greedySplit(const std::string& word) const;

    AnalysisNode buildTree(const std::string& word,
                           std::unordered_set<std::string>& visited) const;
};
