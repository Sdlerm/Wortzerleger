#include "WordBreaker.h"

#include <iostream>

void printTree(const AnalysisNode& node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- " << node.word << " = " << node.meaning << '\n';
    for (const AnalysisNode& child : node.children) {
        printTree(child, depth + 1);
    }
}

int main() {
    WordBreaker breaker;
    breaker.addLinkingElements();

    if (!breaker.loadDictionary("dictionary.txt")) {
        std::cerr << "Could not load dictionary.txt\n";
        return 1;
    }

    std::cout << "German word: ";
    std::string word;
    std::getline(std::cin, word);

    AnalysisNode result = breaker.analyze(word);
    printTree(result);

    return 0;
}
